---
name: codegen
description: Re-run JustReflectMe code generation for reflection/ECS metadata. Use whenever component structs, reflected types, or annotated headers change, or if generated code looks stale.
---

# Regenerate reflection code (JustReflectMe)

Nexium's ECS components self-register via linker sections generated from annotated headers by JustReflectMe.

## When this is needed

- Build errors mentioning missing registration symbols or stale generated headers

## Command

### Linux

```bash
rm .jrm/cache.data
```

### Windows

#### PowerShell

```powershell
Remove-Item .jrm/cache.data
```

#### CMD

```cmd
del .jrm\cache.data
```

## After regenerating

Always follow with a normal `build` (not clean-build) — generated headers changing is exactly what the normal
incremental build is designed to pick up.
