<img src="/documentation/images/hrw_logo.png" align="right" alt="HRW Logo" width="384"/>
<br/>
<br/>
<br/>

# RolliVR

This repository contains the documentation and the software and hardware components of the RolliVR project developed at [Hochschule Ruhr West](https://www.hochschule-ruhr-west.de/). RolliVR aims to create a platform for wheelchair drivers to use their wheelchair to naturally move in existing VR applications and games. For that purpose, we provide a manual along the design files for 3D-prints to create the platform and an OpenVR driver and software package to use the platform in SteamVR applications.

[![Hardware License: TAPR Open Hardware License](https://img.shields.io/badge/Hardware_License-TAPR_Open_Hardware_License-blue)](https://tapr.org/the-tapr-open-hardware-license/)
[![Software License: MIT](https://img.shields.io/badge/Software_License-MIT-blue)](https://opensource.org/licenses/MIT)
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

| Count | Part | Used For |
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

<img src="/documentation/images/encoder-ring.png" align="center" alt="Encoder Ring" width="256"/>

#### Encoder Holder

The encoder holder can be printed with a filament of your choice. ([.stl](hardware/encoder-holder/encoder-holder.stl))

<img src="/documentation/images/encoder-holder.png" align="center" alt="Encoder Holder" width="256"/>

#### Platform Bracket

Both parts of the platform bracket can be printed with a filament of your choice. ([first .stl](hardware/platform-bracket/platform-bracket-1.stl), [second .stl](hardware/platform-bracket/platform-bracket-2.stl))

<img src="/documentation/images/platform-bracket.png" align="center" alt="Platform Bracket" width="256"/>

#### Wheelchair Bracket

Most parts of the wheelchair bracket can be printed with a filament of your choice. ([first .stl](hardware/wheelchair-bracket/wheelchair-bracket-1.stl), [second .stl](hardware/wheelchair-bracket/wheelchair-bracket-2.stl)) The part holding onto the wheelchair should be printed with flexible filament (TPU) for a better grip and to prevent any scratches. ([third .stl](hardware/wheelchair-bracket/wheelchair-bracket-3.stl)) The second and third parts (the parts holding onto the wheelchair) have to be printed twice to form the grip.  

<img src="/documentation/images/wheelchair-bracket.png" align="center" alt="Wheelchair Bracket" width="256"/> 

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

### Required Libraries & Tools

| Software | License | Recommended Installation Method |
| - | - | - |
| Qt5 | LGPLv3 | Download and install from [here](https://www.qt.io/download-qt-installer) |
| OpenVR | BSD 3 | Install via vcpkg |
| GLM | MIT | Install via vcpkg |
| Serial | MIT | Fork integrated as submodule of this repository |
| NSIS | zlib | Download install from [here](https://nsis.sourceforge.io/Download) |
| NSIS Json Plugin | See LICENSE.txt | Included in this repository |
| CMake | New BSD | Download and install from [here](https://cmake.org/download/) |
| Arduino IDE | LGPL | Download and install from [here](https://www.arduino.cc/en/main/software) |


### Driver and Overlay

[CMake](https://cmake.org/download/) is used as build tool. OpenVR and GLM libraries need to be installed on your machine. It is recommended that you download them via [vcpkg](https://github.com/Microsoft/vcpkg). Make sure to install the x64-windows triplets to be able to build the driver and overlay as 64 bit applications:

`vcpkg install openvr glm --triplet x64-windows`

[Qt5](https://www.qt.io/download-open-source) has to be installed to build the overlay, as it is used for the user interface. To build the installer [NSIS](https://nsis.sourceforge.io/Download) has to be installed. With this prerequisites installed and the repository cloned, make sure that you that you have initialized the required submodules via the execution of the following command in the root directory:

`git submodule update --init`

In the software directory, create a build directory and move to it:

`cd software && mkdir build && cd build`

Execute CMake, make sure the TOOLCHAIN_FILE and PREFIX_PATH variables match your installation paths. Set your required release type and specify the generator:

`cmake .. -DCMAKE_BUILD_TYPE=Release -G"Visual Studio 16 2019" -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_PREFIX_PATH=C:\Qt\5.14.2\msvc2017_64\lib\cmake`

Perform the build:

`cmake --build . --config Release`

After the build, the driver folder structure and files will available as `<Path to Repo>\software\build\driver\rollivr` and the overlay folder structure and files will be copied to the output folder as `<Path to Repo>\software\build\overlay\rollivr`.

If you want to create the installer execute the following command:

`cmake --build . --target package --config Release`

The installer will then be located at `<Path to Repo>\build\RolliVR-Installer.exe`.
	
Alternatively, you can access the most recent prebuild installer in the latest completed action.

### Arduino

There are two Arduino projects provided in this repository:

- [arduino-software-debug](embedded/arduino-software-debug) serves as a mean to test the software without the platform. Simply wire two pushbuttons with pullup resistors to the A0 and A5 pins of an Arduino and flash this sketch. Holding down buttons simulates a forward movement of the left and right wheels.

- [arduino-software-encoder](embedded/arduino-software-encoder) is the software required for the real platform. It transmits the movement distance and angle calculated from the values of the optical rotary encoders every 100 milliseconds. It also applies a rolling average filter over 5 samples.

## Usage Guide

After building the installer launch it and install RolliVR to a directory of your choice.

<img src="/documentation/images/usage/setup.png" align="center" alt="Setup" width="512"/>

### Driver

Launch SteamVR after the installation. Connect the Arduino with the firmware flashed to the PC running SteamVR and take a look at the information panel. If no icon is shown at all, make sure an entry in the `external_drivers` array in the `<User>\AppData\Local\openvr\openvrpaths.vrpath` file points to `<RolliVR Installation Directory>\driver\rollivr`. If the RolliVR icon is greyed out, the connection could not be established, if it is blue-greenish, the Arduino is successfully sending data.

<img src="/documentation/images/usage/offline.png" align="left" alt="Offline" width="256"/>
<img src="/documentation/images/usage/online.png" align="center" alt="Online" width="256"/>

In case of a failed connection go to the SteamVR developer console:

<img src="/documentation/images/usage/webconsole.png" align="center" alt="Web Console" width="256"/>

Search for RolliVR and find the logs of available and accepted hardware ids. Available hardware ids are the ids of serial ports connected to your system. Accepted hardware ids are the ids the driver expects.

<img src="/documentation/images/usage/webconsole_1.png" align="center" alt="Web Console" width="512"/>

If your Arduino has a hardware id that is not accepted, add the hardware id and baudrate to the config file located at `<RolliVR Installation Directory>\driver\rollivr\resources\config\comports.txt`. Restart SteamVR afterwards. The Arduino should successfully connect and you can test the controller by accessing `#/USER/TREADMILL` under Controllers > Test Controller. VR Applications that allow custom controller bindings can now be configured to use the RolliVR controller as an input.

<img src="/documentation/images/usage/test_controller.png" align="center" alt="Test Controller" width="512"/>

### Overlay

When using roomscale VR applications with RolliVR, the overlay is required. To make sure the overlay has correctly registered itself as an autostart application, go to Choose Startup Overlay Apps under Startup / Shutdown. If RolliVR is displayed, you are good to continue, otherwise run `install_manifest.bat` in the installation directory and restart SteamVR. RolliVR should now be correctly registered and enabled as autostart application:

<img src="/documentation/images/usage/startup.png" align="center" alt="Autostart Applications" width="512"/>

The driver and the overlay need to be connected next. Go to the Controllers and make sure advanced settings are shown. Click on Show Old Binding UI.

<img src="/documentation/images/usage/controllers.png" align="center" alt="Controllers Menu" width="512"/>

Now proceed to edit the bindings of RolliVR. You may need to click on Show More Applications before it shows up.

<img src="/documentation/images/usage/bindingui.png" align="center" alt="Binding UI" width="512"/>

Firstly, change the controller used for the RolliVR application. This defaults to your VR headsets controllers, but it needs to be changed to the RolliVR Controllers. To do so, click on the name of the Current Controller and select the RolliVR Controller in the following menu.

<img src="/documentation/images/usage/bindingui_1.png" align="center" alt="Binding UI" width="512"/>

<img src="/documentation/images/usage/bindingui_2.png" align="center" alt="Binding UI" width="512"/>

Secondly, activate the default RolliVR Overlay bindings for the RolliVR controller under Official Bindings.

<img src="/documentation/images/usage/bindingui_3.png" align="center" alt="Binding UI" width="512"/>

With that step done, the overlay is fully functional and you can access it in VR using the SteamVR menu. The RolliVR icons appears on the lower left corner.

<img src="/documentation/images/usage/overlay.png" align="center" alt="Overlay" width="512"/>

To start moving in roomscale VR applications, push the Start Forced Movement button while seated on the platform and looking straight ahead. The menu must be closed for the movement to work.

<img src="/documentation/images/usage/overlay_1.png" align="center" alt="Overlay" width="384"/>

Inputs to the wheelchair now move you around the room. To reset your movement and return to the origin, push Reset Accumulated Movement. To stop the movement with the wheelchair, push Stop Forced Movement. Turn and Movement Speeds as well as a height offset can be configured in this menu, too.

<img src="/documentation/images/usage/overlay_2.png" align="center" alt="Overlay" width="384"/>
