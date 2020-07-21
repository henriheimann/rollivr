<img src="/documentation/images/hrw_logo.png" align="right" alt="HRW Logo" width="384"/>
<br/>
<br/>
<br/>

# RolliVR

This repository contains the documentation and the software and hardware components of the RolliVR project developed at [Hochschule Ruhr West](https://www.hochschule-ruhr-west.de/). RolliVR aims to create a platform for wheelchair drivers to use their wheelchair to naturally move in existing VR applications and games.

[![License: TAPR Open Hardware License](https://img.shields.io/badge/License-TAPR_Open_Hardware_License-blue)](https://tapr.org/the-tapr-open-hardware-license/)
[![Build Status](https://github.com/henriheimann/rollivr/workflows/build/badge.svg)](https://github.com/henriheimann/rollivr/actions)

</br>

<img src="/documentation/images/platform.jpg" align="center" alt="Platform" width="800"/>

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

| Count | Part | Used for |
| - | - | - |
| 1 | Arduino Uno (or similar) | electronics |
| 1 | USB cable for the chosen Arduino | electronics |
| 1 | USB extension cable | electronics |
| 2 | Grove optical rotary encoder (TCUT1600X01) | electronics |
| 2 | Grove universal 4 pin cable | electronics |
| - | jumper wires | electronics |
| 2 | film coated plywood 1500x7500mm (thickness 18mm) | platform |
| 2 | hinges | platform |
| 8 | height adjustable feet (height around 10mm) | platform |
| 2 | load bearing rollers (length 200mm, diameter 80mm, 12mm thread) | platform |
| 8 | stainless steel angle brackets | platform |
| 8+ | M12 nuts | platform |
| 40+ | M5 pointed screws | platform |
| 2 | telescopic tubes (length around 1m, diameter 18mm to 22.5mm) | wheelchair bracket |
| 8+ | M3 screws (length 40mm) | wheelchair bracket |
| 8+ | M3 nuts | wheelchair bracket |
| 8+ | M3 washers | wheelchair bracket |
| 6+ | M5 screws (length 40mm) | wheelchair bracket |
| 6+ | M5 nuts | wheelchair bracket |
| 6+ | M5 washers | wheelchair bracket |
| - | flexible filament (TPU) | wheelchair bracket |
| - | dark filament | encoder disk |
| - | any filament | other 3D-printed parts |
| - | double sided tape | encoder disk |
| - | duct tape | wiring |


### 3D-Printed Components

#### Encoder Ring

The encoder ring must be printed with a dark filament to properly block the light of the rotary encoder's LEDs. ([.stl](hardware/encoder-ring/encoder-ring.stl))

<img src="/documentation/images/encoder-ring.png" align="center" alt="Encoder Ring" width="384"/>

#### Encoder Holder

The encoder holder can be printed with a filament of your choice. ([.stl](hardware/encoder-holder/encoder-holder.stl))

<img src="/documentation/images/encoder-holder.png" align="center" alt="Encoder Holder" width="384"/>

#### Platform Bracket

Both parts of the platform bracket can be printed with a filament of your choice. ([first .stl](hardware/platform-bracket/platform-bracket-1.stl), [second .stl](hardware/platform-bracket/platform-bracket-2.stl))

<img src="/documentation/images/platform-bracket.png" align="center" alt="Platform Bracket" width="384"/>

#### Wheelchair Bracket

Most parts of the wheelchair bracket can be printed with a filament of your choice. ([first .stl](hardware/wheelchair-bracket/wheelchair-bracket-1.stl), [second .stl](hardware/wheelchair-bracket/wheelchair-bracket-2.stl)) The part holding onto the wheelchair should be printed with flexible filament (TPU) for a better grip and to prevent any scratches. ([third .stl](hardware/wheelchair-bracket/wheelchair-bracket-3.stl)) The second and third parts (the parts holding onto the wheelchair) have to be printed twice to form the grip.  

<img src="/documentation/images/wheelchair-bracket.png" align="center" alt="Wheelchair Bracket" width="384"/> 

### Assembly

#### Platform

Firstly, cut the two film coated plywood boards into semicircles with a diameter of 750mm. Align them and use two hinges to combine the two halves into a complete circle. This allows for easier transport of the platform. Cut out two holes which will later house the load bearing rollers according to the following drawing.

<img src="/documentation/images/platform-drawing.png" align="center" alt="Platform Drawing" width="384"/>

Evenly space out and screw the eight height adjustable feet to the bottom of the board to elevate it to a height of about 10cm.

#### Rollers

Place the 3d-printed encoder rings over two of the rollers and fixate them using double sided tape on their bottom side.

Use a single screw to fix the optical rotary encoder onto the 3d printed holder. Anchor each load bearing roller between two angle brackets putting the rotary encoder holders (where applicable) and nuts in between as seen in the image below.

<img src="/documentation/images/rollers.jpg" align="center" alt="Rollers" width="800"/>

Screw the angle brackets to the platform afterwards.

#### Bracket

Assemble the wheelchair and platform brackets using the M5 and M3 screws, nuts and washers as seen in the image below.

<img src="/documentation/images/bracket.jpg" align="center" alt="Bracket" width="800"/>

### Electronics

Both optical encoders need to be connected to the Arduino's VCC and ground pins and the respective digital input pins 6 to 9 as shown in the image below.

<img src="/documentation/images/wiring.jpg" align="center" alt="Wiring" width="800"/>

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
