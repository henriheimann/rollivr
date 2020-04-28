# Wheelchair OpenVR Controller

## Building

### Driver and Overlay

- OpenVR libraries need to be installed
    - Recommended installation via https://github.com/Microsoft/vcpkg
    - Make sure to install x64-windows triplets
- Qt5 has to be installed (https://www.qt.io/download-open-source)
- Execute CMake `cd ovr-wheelchair-driver && cmake . -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_PREFIX_PATH=C:\Qt\5.14.2\msvc2017_64\lib\cmake`
- Build with Visual Studio, Clion or the IDE of your choice
	- Driver folder structure and files will be copied to the output folder as `wheelchair`.
	
### Embedded Software

- Build and deploy via the Arduino IDE
	
## Installation

- Navigate to `C:\Users\<Username>\AppData\Local\openvr` and find the `openvrpaths.vrpath` file. Open this file with your text editor of choice, and under `"external_drivers"`, add another entry with the location of the `wheelchair` folder. For example mine looks like this after adding the entry:

```json
{
	"external_drivers" : 
	[
		"C:\\<Path to Repo>\\openvr-driver\\build\\driver\\wheelchair"
	]
}
```