# Nexium Engine

## 3D cross-platform game engine
**Nexium Engine is a feature-packed, cross-platform game engine to create 3D games from a unified interface. It provides a comprehensive set of common tools so that users can focus on making games without having to reinvent the wheel.** Now, it is currenty under active development. In the future games can be exported with one click to a number of platforms, including the major desktop platforms (Linux, macOS, Windows), mobile platforms (Android, iOS)

## Free, open source
Nexium is completely free and open source under the very permissive MIT license. No strings attached, no royalties, nothing. The users' games are theirs, down to the last line of engine code. Nexium's development is fully independent, and it is not intended for profit.

99.9% of Nexium Engine was developed by Valerii Koniushenko. Also, it has previous versions, mainly: VaKon2D, VaKon3D, Segalay - you can find it on the author's repositories [Valerii Koniushenko](https://github.com/ValeriiKoniushenko).

<img width="1832" height="1009" alt="image" src="https://github.com/user-attachments/assets/cfe20fe5-d5a8-47ec-9b8f-0233c77e5fa3" />

## Getting the engine

### Binary downloads
Now, we do not have such options. Because it is improving and modifying almost every day. But you can build it on your own machine from the source code.

### Compiling from source

It is very easy. Everything that you need it is just clone the repository and using cmake 3.6> build a project. More details you can find below.

### Windows

#### Pre-requirements

1. ```CMake 3.6``` >
2. ```MSVC 14``` >

#### Building

```bat
git clone https://github.com/ValeriiKoniushenko/Nexium.git
cd Nexium
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build
```

### Linux

#### Pre-requirements

1. You need to install the next components to build the project.
   ```
   sudo apt install libwayland-dev libx11-dev libxi-dev libxrandr-dev libxinerama-dev libxkbcommon-dev libxcursor-dev libgl1-mesa-dev
   ```
2. ```CMake 3.6``` >
3. ```Clang 19``` > or ```GCC 14``` >

#### Building

```sh
git clone https://github.com/ValeriiKoniushenko/Nexium.git
cd Nexium
cmake -S . -B build
cmake --build build
```

## Documentation and demos
Take into account, due to active development, the real game & engine were not divided. So, now you can write your own game as a part of the engine. It will be fixed in the nearest future.
Also, you can find a Demo Game in the root of the project under the name 'game-example'.~~~~ After successfully building
you can run it by the next path: ```<build_dir>/bin/TemplateGame```.

About documentation: the same reason. After the first beta version, all documentation will be added here.

## Stability

[![Jenkins](https://img.shields.io/badge/jenkins-%232C5263.svg?style=for-the-badge&logo=jenkins&logoColor=white)](https://jenkins.vakon.space/job/Nexium/)

**Builds**:

- [![MSVC Debug](https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fjenkins.vakon.space%2Fjob%2FNexium%2Fjob%2FWinBuild_MSVC_Debug%2F&label=MSVC%20Debug)](https://jenkins.vakon.space/job/Nexium/job/WinBuild_MSVC_Debug/) [![MSVC Release](https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fjenkins.vakon.space%2Fjob%2FNexium%2Fjob%2FWinBuild_MSVC_Release%2F&label=MSVC%20Release)](https://jenkins.vakon.space/job/Nexium/job/WinBuild_MSVC_Release/)
- [![GCC Debug](https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fjenkins.vakon.space%2Fjob%2FNexium%2Fjob%2FLinuxBuild_GCC_Debug%2F&label=GCC%20Debug)](https://jenkins.vakon.space/job/Nexium/job/LinuxBuild_GCC_Debug/) [![GCC Release](https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fjenkins.vakon.space%2Fjob%2FNexium%2Fjob%2FLinuxBuild_GCC_Release%2F&label=GCC%20Release)](https://jenkins.vakon.space/job/Nexium/job/LinuxBuild_GCC_Release/)
- [![Clang Debug](https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fjenkins.vakon.space%2Fjob%2FNexium%2Fjob%2FLinuxBuild_Clang_Debug%2F&label=Clang%20Debug)](https://jenkins.vakon.space/job/Nexium/job/LinuxBuild_Clang_Debug/) [![Clang Release](https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fjenkins.vakon.space%2Fjob%2FNexium%2Fjob%2FLinuxBuild_Clang_Release%2F&label=Clang%20Release)](https://jenkins.vakon.space/job/Nexium/job/LinuxBuild_Clang_Release/)


