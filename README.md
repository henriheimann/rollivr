# Wheelchair OpenVR Driver

## Building
- Clone the project and submodules
	- `git clone --recursive <Repo URL>`
- Build project with CMake
	- `cd ovr-wheelchair-driver && cmake .`
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