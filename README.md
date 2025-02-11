# ProgressProject
Progress Project is a tool which monitors a directory and uploads new files to MOVEit.

To run locally:
    - As a prerequisite, you should have CMake and vcpkg installed before trying to run this project.
        - For CMake check out their website on https://cmake.org/download/ for your platform.
          You should add CMake to your $PATH and you should set CXX as the path to your compiler.
          This was developed using g++ so I recommend using that.
        
        - For vcpkg:
            - Clone the repository from https://github.com/microsoft/vcpkg.git
            - Go into vcpkg directory and run ./bootstrap-vcpkg.sh (for Linux/MacOS) or ./bootstrap-vcpkg.bat (Windows)
            - Integrate with CMake ./vcpkg integrate install
            - Clone the project (ProgressProject) and run ./vcpkg install (this should install all dependencies)
   
     - After these steps, create a build directory and run:
        - cd build
        - cmake ..
        - make
     - Run the ProgressProject binary in the build directory