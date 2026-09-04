# Shared C++ CI for Gitea Actions

This directory is the source of a future standalone shared-CI repository.  It
contains the portable code extracted from Nexium, Utils, and JustReflectMe.
Nexium's project configuration intentionally lives outside this future
repository at `.ci/cpp-ci.json`.
It deliberately contains no project executable name, CMake option, or
project-specific suppression policy.

## What the required pipeline runs

The generic workflow performs these independent jobs:

```text
clang-format     clang-tidy (own clang-tidy build dir)
      \                 /
       \               /
 clang Debug / Release     gcc Debug / Release
        |        |          |          |
      unit test for every compiler/configuration
        |
    Valgrind unit tests (clang Debug)
        |
optional configured application capture + Valgrind (clang Debug)
```

Every profile has a fresh, job-local directory:

```text
build/ci/clang-debug
build/ci/clang-release
build/ci/gcc-debug
build/ci/gcc-release
build/ci/clang-tidy
```

There is intentionally no persistent CMake/Ninja build-tree mount.  Those
trees embed a checkout path, submodule state, generator, compiler, and source
configuration, so sharing them is the cause of the `/repo-cache/.git/config`
and nested-submodule lock failures.  The only persistent compilation cache is
`/ccache`, which is safe for concurrent ccache clients.  Every configure call
unconditionally supplies both C and C++ ccache launchers, and each build
prints ccache statistics.

The clang-tidy and build jobs also mount a persistent Git metadata cache at
`/submodule-cache`.  It stores a project-scoped copy of `.git/modules`, never
a worktree or a CMake build directory.  A job whose pinned submodule graph
matches the cache copies that metadata locally and checks out without network
access.  When a pin changes, one locked job uses the previous metadata as a
starting point, fetches only what is missing, and atomically refreshes the
cache; concurrent jobs then use the refreshed copy.  This preserves the exact
gitlinks committed by the consumer project—there is no periodic
`git submodule update --remote` policy and no stale third-party dependency
state.

The runner must allow and persist the host bind mount
`/srv/ci-cache/cpp-ci-submodules:/submodule-cache` (including the matching
`valid_volumes` entry when act_runner uses an allowlist).  Keep the host cache
root private to trusted CI jobs.  The helper hashes the consumer repository
identity, so projects sharing this mount receive separate cache directories.

## Consumer layout

After this directory is made into its own repository, add it as a submodule at
`.gitea/ci` in each consuming project.  Keep the project configuration outside
the submodule:

```text
.ci/cpp-ci.json                 # project-owned policy
.gitea/ci/                      # shared-CI submodule
.gitea/workflows/code-quality.yml # project-owned discovery stub
valgrind.supp                   # optional project-owned suppressions
```

Use [`templates/project-config.json`](templates/project-config.json) as the
starting point; working configurations for all three repositories are in
[`examples/`](examples/).  Central defaults, including the selected Clang,
clang-format, clang-tidy, GCC, Valgrind, and default concurrency, live in
[`config/toolchains.json`](config/toolchains.json). The common workflow itself
centrally owns the container-image reference and mounts `/ccache`; local runs
keep their normal ccache directory unless `CCACHE_DIR` is explicitly set. A
project may override a field only when it genuinely needs a different toolchain.

`cmake.arguments` applies to every profile. Use
`cmake.profile_arguments.clang-debug` (and the other profile names) only for
configuration that genuinely differs by compiler or build mode.

`templates/consumer-workflow.yml` is the preferred tiny consumer workflow. It
calls this repository's reusable workflow at an immutable tag and passes the
two paths above.  That lets the common repository centrally control the job
graph, container image, and tool defaults.

While this directory is still Nexium's `.gitea` directory, the source workflow
also accepts `pull_request`, `push` to `develop`, and manual-dispatch events.
It falls back to `.gitea` and `.ci/cpp-ci.json`, so the current Nexium pull
request runs immediately. After extracting this directory into the standalone
shared-CI repository, install `templates/consumer-workflow.yml` in each
consumer; that wrapper owns those triggers and calls the shared workflow.

For an application-specific visual smoke check, enable the `runtime` section
in the project config. The generic capture and application-Valgrind jobs then
restore the Clang Debug runtime artifact and run automatically. Projects with
`runtime.enabled: false` report a successful skip; no job name or window title
is baked into the common layer.

Gitea discovers workflow YAML from the parent repository's Git tree before a
runner checks out submodules.  Consequently, a workflow stored only inside
`.gitea/ci` cannot be discovered as a local workflow.  The small parent stub
is therefore required.  Gitea 1.27 provides same-instance reusable workflows;
if cross-repository read access is unavailable, keep an equivalent generated
wrapper in each parent repository instead.  Scoped workflows are a server-wide
alternative.  See the [Gitea 1.27 release notes](https://blog.gitea.com/release-of-1.27.0/)
and [scoped workflow documentation](https://docs.gitea.com/next/usage/actions/scoped-workflows/).

## Migration handoff

No Git operation is performed by this CI source tree. When you create the
shared repository, keep its contents intact, then place that repository at
`.gitea/ci` in Nexium, Utils, and JustReflectMe. For each consumer:

1. Keep/create `.ci/cpp-ci.json` from the matching example.
2. Copy `templates/consumer-workflow.yml` to the parent repository's
   `.gitea/workflows/code-quality.yml` and replace the repository name and
   immutable ref.
3. Ensure the consumer can read the private shared repository in Gitea Actions
   settings, and provide one `GITEATOKEN` secret with permission to publish
   reviews/statuses. The workflow deliberately maps it to both `GITEATOKEN`
   and `GITEA_TOKEN` for compatible shared helpers.
4. Delete that consumer's old copied Python CI helpers only after its first
   shared workflow run succeeds.

The `Vakon/cpp-ci` placeholder is intentionally not a command or a remote
operation; replace it with the name you choose for the new repository.

## Local commands

Run these from a consumer repository root (replace `.gitea/ci` with `.gitea`
while developing this source directory in Nexium):

```sh
python3 .gitea/ci/bin/cpp-ci.py configure --config .ci/cpp-ci.json --profile clang-debug
python3 .gitea/ci/bin/cpp-ci.py build --config .ci/cpp-ci.json --profile clang-debug
python3 .gitea/ci/bin/cpp-ci.py format --config .ci/cpp-ci.json --base develop --no-gitea
python3 .gitea/ci/bin/cpp-ci.py tidy --config .ci/cpp-ci.json --files sources/Foo.cpp --no-gitea
python3 .gitea/ci/bin/cpp-ci.py valgrind --config .ci/cpp-ci.json --profile clang-debug --no-gitea
```

## Valgrind suppressions

The unit-test Valgrind command always supports project suppression files and,
by default, adds `--gen-suppressions=all`.  Thus every unsuppressed false
positive is printed in a copyable Valgrind suppression block in the job log.
Add only the narrowly matching block to the consumer's `valgrind.supp`, then
rerun the same check.  Suppression paths are declared in the project's
`valgrind.suppressions` array; an empty array is valid.

For a focused local reproduction:

```sh
python3 .gitea/bin/cpp-ci.py valgrind \
  --config .ci/cpp-ci.json --profile clang-debug --no-gitea -- \
  --gtest_filter='RelevantSuite.*'
```

## Optional application capture

`cpp_ci.runtime` is generic: it accepts an arbitrary command, warm-up time,
screen size, environment variables, and optionally a window title.  Root-window
capture needs the supplied image's `xvfb` and `scrot`; title-specific capture
also needs `xdotool` and ImageMagick's `import` command.

```sh
PYTHONPATH=.gitea python3 -m cpp_ci.runtime capture-window \
  --output app.png --warmup 5 -- --path/to/application --its-arguments
```

Application Valgrind runs use the analogous `run-under-valgrind` command.  A
consumer's optional runtime job decides its executable and renderer-specific
environment; the shared code does not encode `TemplateGame`, a window cache,
or a renderer policy.
