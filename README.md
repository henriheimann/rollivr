# Wheelchair OpenVR Controller

## Building

### Driver and Overlay

- OpenVR libraries need to be installed
    - Recommended installation via https://github.com/Microsoft/vcpkg
    - Make sure to install x64-windows triplets
- Qt5 has to be installed
    - Download from https://www.qt.io/download-open-source
    - Set environment variables `QT_QMAKE_EXECUTABLE`, `Qt5_DIR` and `Qt5Core_DIR` or tell CMake about Qt another way
- Build project with CMake (omit `-DCMAKE_TOOLCHAIN_FILE` if you are not using vcpkg)
	- `cd ovr-wheelchair-driver && cmake . -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake`
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