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

## Readme Contents

- [Building the Platform](#building-the-platform)
    - [Bill of Materials](#bill-of-materials)
    - [3D-Printed Components](#3D-Printed-Components)
    - [Electronics](#electronics)
    - [Assembly](#assembly)
- [Building the Software](#building-the-software)
    - [Required Software](#required-software)
    - [Arduino](#arduino)
    - [Driver and Overlay](#driver-and-overlay)
- [Usage Guide](#usage-guide)

## Building the Platform

### Bill of Materials

| Count | Part |
| - | - |
| 1 | Arduino Uno (or similar) |
| 2 | Grove Optical Rotary Encoder (TCUT1600X01) |
| 2 | Grove Universal 4 Pin Cable |
| - | Jumper Wires |


### 3D-Printed Components


#### Encoder Ring

<img src="/documentation/images/encoder-ring.png" align="center" alt="Encoder Ring" width="384"/>

#### Encoder Holder

<img src="/documentation/images/encoder-holder.png" align="center" alt="Encoder Holder" width="384"/>

#### Platform Bracket

<img src="/documentation/images/platform-bracket.png" align="center" alt="Platform Bracket" width="384"/>

#### Wheelchair Bracket

<img src="/documentation/images/wheelchair-bracket.png" align="center" alt="Wheelchair Bracket" width="384"/>

### Electronics    

### Assembly


## Building the Software

### Required Software

### Arduino

### Driver and Overlay

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

## Usage Guide

- The driver and overlay application should automatically launch with SteamVR
    - If the overlay doesn't start run install_manifest.bat and restart SteamVR
