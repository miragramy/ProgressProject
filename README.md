# ProgressProject
Progress Project is a tool which monitors a directory and uploads new files to MOVEit.

To run locally:
     - As a prerequisite, you should have `CMake`, `vcpkg`, `make` installed before trying to run this project.
        - For CMake check out their website on https://cmake.org/download/ for your platform.
          You should add CMake to your $PATH and you should set CXX as the path to your compiler.
          This was developed using g++ so I recommend using that.
        
        - For vcpkg:
            - Clone the repository from https://github.com/microsoft/vcpkg.git
            - Go into vcpkg directory and run `./bootstrap-vcpkg.sh` (for Linux/MacOS) or `./bootstrap-vcpkg.bat` (Windows)
            - Integrate with CMake `./vcpkg integrate install`
            - For MacOS/Linux you have to install `brew install pkg-config` and for Windows you have to install Visual Studio 
                (Desktop development for C++, Cmake Tools for Windows, Windows 10/11 SDK, MSVC v143 or later)
            - For Windows, you should also set the following environment variable $env:VCPKG_DEFAULT_TRIPLET="x64-mingw-static" since you're going to
              be using the MSVC compiler and vcpkg defaults to x64-windows triplet
            - Clone the project (ProgressProject) and run `<PATH_TO_vcpkg>/vcpkg install` from inside the project directory.

        - For make:
            - On MacOS/Linux, you probably already have it installed or can use some package manager like brew to install it
            - For Windows, you have to use MinGW toolchain, which has make in it. You can follow https://code.visualstudio.com/docs/cpp/config-mingw
   
     - After these steps, create a build directory and run:
        - For MacOS/Linux
            - `mkdir build`
            - `cd build`
            - `cmake ..`
            - `make`
        - For Windows
            - `mkdir build`
            - `cmake -B build -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=<Path/to/vcpkg>/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake  -DVCPKG_TARGET_TRIPLET=x64-mingw-static`
            - `cd build`
            - `<path\to\mingw32-make.exe>` (if you're using MinGW toolchain, otherwise just run whatever version of make you have)
     - Run the ProgressProject binary in the build directory