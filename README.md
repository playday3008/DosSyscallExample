# DOS Syscall Example

Minimal DOS program demonstrating direct INT 21h system calls using the Open Watcom C/C++ compiler. Supports both 16-bit real mode and 32-bit DOS/4GW protected mode targets.

## Building

Requires [Open Watcom](https://open-watcom.github.io/) and CMake 3.21+.

```bash
cmake --preset dos32-debug --fresh
cmake --build --preset dos32-debug
```

Available presets: `dos32-debug`, `dos32-release`, `dos16-debug`, `dos16-release`.

Output: `build/dossyscl.exe`

## Overview

Two syscall techniques are demonstrated in `src/dossyscall.c`:

- **Library-based** (`dos_get_version`) -- uses `int86()` / `int386()` to invoke INT 21h/AH=30h.
- **Inline assembly** (`dos_get_time`) -- uses Watcom `#pragma aux` to emit inline INT 21h/AH=2Ch with explicit register bindings.

Both paths compile for 16-bit and 32-bit targets via `#if defined(_M_I86)`.
