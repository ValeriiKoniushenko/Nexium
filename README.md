# Nexium Engine

## 3D cross-platform game engine

**Nexium Engine is a feature-packed, cross-platform game engine to create 3D games from a unified interface. It provides
a comprehensive set of common tools so that users can focus on making games without having to reinvent the wheel.** Now,
it is currently under active development. In the future games can be exported with one click to a number of platforms,
including the major desktop platforms (Linux, macOS, Windows), mobile platforms (Android, iOS)

## Free, open source

Nexium is completely free and open source under the very permissive MIT license. No strings attached, no royalties,
nothing. The users' games are theirs, down to the last line of engine code. Nexium's development is fully independent,
and it is not intended for profit.

99.9% of Nexium Engine was developed by Valerii Koniushenko. Also, it has previous versions, mainly: VaKon2D, VaKon3D,
Segalay – you can find it on the author's repositories [Valerii Koniushenko](https://github.com/ValeriiKoniushenko).

<img width="1832" height="1009" alt="image" src="https://github.com/user-attachments/assets/cfe20fe5-d5a8-47ec-9b8f-0233c77e5fa3" />

## Getting the engine

### Windows

#### Pre-requirements

1. `CMake 3.6` >
2. `MSVC 14` >
3. Opt: `ccache 4.x` >

#### Building

```bat
git clone https://github.com/ValeriiKoniushenko/Nexium.git
cd Nexium
git submodule update --init --recursive --remote
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build
```

### Linux

#### Pre-requirements

1. You need to install the next components to build the project.
   ```
   sudo apt install libwayland-dev wayland-protocols libxkbcommon-dev \ 
    libxkbcommon-x11-dev libx11-dev libxrandr-dev libxinerama-dev \
    libgl1-mesa-dev pkg-config libxcursor-dev libxi-dev
   ```
2. `CMake 3.6` >
3. `Clang 22` >= or `GCC 14` >=
4. Set `git nuke` alias and execute it after every checkout:
    - **Linux** shell:
         ```sh
         git config alias.nuke '!rm -f .jrm/cache.data && find -name '*.generated.*' -delete && git submodule sync --recursive && git submodule update --init --recursive --remote'
         ```
    - **Windows** PowerShell:
        ```pwsh
        git config alias.nuke '!powershell -NoProfile -Command "Remove-Item -Force -ErrorAction SilentlyContinue .jrm/cache.data; Get-ChildItem -Recurse -Filter '\''*.generated.*'\'' -File | Remove-Item -Force; git submodule sync --recursive; git submodule update --init --recursive --remote"'
        ```
5. Opt: `ccache 4.x` >
6. Opt: `mold 2.x` (linker)

#### Building

```sh
git clone https://github.com/ValeriiKoniushenko/Nexium.git
cd Nexium
git submodule update --init --recursive --remote
cmake -S . -B build
cmake --build build
```

### Opt: 10x build speed

We will speed up ccache to increase the build speed. Populate the next config at your `ccache.conf` file. Default
locations:

- Windows: `%APPDATA%\ccache\ccache.conf`
- Linux: `~/.config/ccache/ccache.conf`

```
hash_dir = false
sloppiness = include_file_mtime,include_file_ctime,time_macros,locale
depend_mode = true
compression = true
max_size = 20G
```

### Opt: 2x link speed (Linux)

```sh
# install 'mold' linker
sudo apt install mold
```

## Documentation and demos

Take into account, due to active development, the real game & engine were not divided. So, now you can write your own
game as a part of the engine. It will be fixed in the nearest future. Also, you can find a Demo Game in the root of the
project under the name 'game-example'. After successfully building
you can run it by the next path: ```<build_dir>/bin/TemplateGame```.

About documentation: the same reason. After the first beta version, all documentation will be added here.
