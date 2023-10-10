# NES Project for FLC++

This presentation runs on an NES.

Compiler: **[LLVM-MOS](https://www.llvm-mos.org/)**
> Downloads for desktop platforms: https://github.com/llvm-mos/llvm-mos-sdk#download

Graphics: **[YY-CHR.NET](https://www.romhacking.net/utilities/958/)**
Lots of other options online, this one has been my favourite.

Other options for C: [CC65](https://cc65.github.io), x86 translation

## Requirements

- [CMake](https://cmake.org)
- [Ninja](https://ninja-build.org) (recommended from brew, pacman, winget, etc)
- Download LLVM-MOS into `./llvm-mos`. Be sure that it does not have a second `llvm-mos` folder inside.

VS Code integration expects the following extensions:

- C/C++
- CMake, CMake Tools

## Building

From command line:

```sh
# once:
cmake -G Ninja
# evey time afer:
ninja
```

From VS Code:

1. Configure the CMake project.
2. Select the `[LLVM MOS NES Clang 18]` Kit
3. Either:
    - Run the `CMake: Build` action (F7) (output to `/build/FLCXXNES.NES`)
    - Run the default build task (Ctrl+Shift+B) (output to `FLCXXNES.NES`)

## Running

Recommended emulator: [Mesen](https://mesen.ca)