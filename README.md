# Wheelchair OpenVR Driver

## Building

- A valid binary distribution of github.com/wjwwood/serial (included for Windows x64) must exist in the lib folder
    - Serial can currently not be easily build without catkin installed (use github.com/blabdouze/serial)
- OpenVR libraries need to be installed
    - Recommended installation via https://github.com/Microsoft/vcpkg
    - Make sure to install x64-windows triplets
    - Clone the project and submodules
	- `git clone --recursive <Repo URL>`
- Build project with CMake (omit `-DCMAKE_TOOLCHAIN_FILE` if you are not using vcpkg)
	- `cd ovr-wheelchair-driver && cmake . -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake`
- Build with Visual Studio
	- Driver folder structure and files will be copied to the output folder as `wheelchair`.
	
## Installation

- Navigate to `C:\Users\<Username>\AppData\Local\openvr` and find the `openvrpaths.vrpath` file. Open this file with your text editor of choice, and under `"external_drivers"`, add another entry with the location of the `wheelchair` folder. For example mine looks like this after adding the entry:

```json
{
	"external_drivers" : 
	[
		"C:\\<Path to Repo>\\ovr-wheelchair-driver\\build\\Debug\\wheelchair"
	]
}
```