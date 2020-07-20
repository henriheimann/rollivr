<img src="/documentation/images/hrw_logo.png" align="right" alt="HRW Logo" width="384"/>
<br/>
<br/>
<br/>

# RolliVR

This repository contains the documentation and the software and hardware components of the RolliVR project developed at [Hochschule Ruhr West](https://www.hochschule-ruhr-west.de/). RolliVR aims to create a platform for wheelchair drivers to use their wheelchair to naturally move in existing VR applications and games.

[![License: TAPR Open Hardware License](https://img.shields.io/badge/License-TAPR_Open_Hardware_License-blue)](https://tapr.org/the-tapr-open-hardware-license/)
[![Build Status](https://github.com/henriheimann/rollivr/workflows/build/badge.svg)](https://github.com/henriheimann/rollivr/actions)

</br>

<img src="/documentation/images/platform.jpg" align="center" alt="Platform" width="600"/>

</br>
</br>

## README Contents

1. [Building the Platform](#1.-building-the-platform)
    1. [Bill of Materials](#1.1.-bill-of-materials)
    2. [3D-Printed Components](#1.2.-3D-Printed-Components)
    3. [Electronics](#1.3.-electronics)
    4. [Assembly](#1.4.assembly)
2. [Building the Software](#2.-building-the-software)
    1. [Required Software](#2.1.-erequired-softwar)
    2. [Arduino](#2.2.-arduino)
    3. [Driver and Overlay](#2.3.-driver-and-overlay)
3. [Usage Guide](#3.-usage-guide)

## 1. Building the Platform

### 1.1. Bill of Materials

| Count | Part |
| - | - |
| 1 | Arduino Uno (or similar) |
| 2 | Grove Optical Rotary Encoder (TCUT1600X01) |
| 2 | Grove Universal 4 Pin Cable |
| - | Jumper Wires |


### 1.2. 3D-Printed Components

### 1.3. Electronics    

### 1.4. Assembly


## 2. Building the Software

### 2.1. Required Software

### 2.2. Arduino

### 2.3. Driver and Overlay

- OpenVR and GLM libraries need to be installed
    - Recommended installation via https://github.com/Microsoft/vcpkg
    - Make sure to install x64-windows triplets
- Qt5 has to be installed (https://www.qt.io/download-open-source)
- Execute CMake `mkdir build && cd build && cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_PREFIX_PATH=C:\Qt\5.14.2\msvc2017_64\lib\cmake`
- Build with Visual Studio, Clion or the IDE of your choice
    - The package target creates an installer at `<Path to Repo>\build\RolliVR-Installer.exe`
	- Driver folder structure and files will available as `<Path to Repo>\build\driver\rollivr`.
	- Overlay folder structure and files will be copied to the output folder as `<Path to Repo>\build\overlay\rollivr`.
	
Alternatively, you can access the most recent prebuild installer in the latest completed action.

## 3. Usage Guide

- The driver and overlay application should automatically launch with SteamVR
    - If the overlay doesn't start run install_manifest.bat and restart SteamVR
