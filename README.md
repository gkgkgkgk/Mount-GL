# Mount-GL
My attempt to simulate and visualize the formation of mountains with C++ and OpenGL.

## How to Run
I am still getting used to OpenGL myself, so I'm sure the process to run this could be way better. For now, I am running on WSL2 along VcXsrv and followed these steps.

* Launch a new display with XLaunch. Make sure to disable "Native opengl" and enable "Disable access control" in the Extra Settings.
* Set this environment variable: `export LIBGL_ALWAYS_INDIRECT=0`
* Find and copy your display IP by running `cat /etc/resolv.conf`
* Set this environment variable with your IP `export DISPLAY=IPADDRESS:0.0`
* Set this environment variable: `export MESA_GL_VERSION_OVERRIDE=3.3`
* Create a build folder
* In the build folder, run `cmake ..`
* Run `make`
* Run the executable

## References
* [How to set up VCXsrv](https://www.youtube.com/watch?v=4SZXbl9KVsw)
* [Project structure was based on this repo](https://github.com/carl-vbn/opengl-gravity-simulator)