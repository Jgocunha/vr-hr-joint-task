# vr-hr-joint-task

Project in Human Robot joint action at TU/e.

## Dependencies

### CMake

https://cmake.org/

### vcpkg

https://github.com/microsoft/vcpkg?tab=readme-ov-file#quick-start-windows

```bash
git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.bat
```

Set the VCPKG_ROOT environment variable (C:/.../vcpkg).

### imgui, implot, and nhlomann-json w/ vcpkg (optional)

Optional step. This will be done automatically when running the build.bat file.

```bash
./vcpkg/vcpkg install install imgui[docking-experimental,core,dx12-binding,win32-binding]:x64-windows
./vcpkg/vcpkg install install install implot:x64-windows
./vcpkg/vcpkg install install nlohmann-json:x64-windows
```

### dynamic-neural-field-composer

https://github.com/Jgocunha/dynamic-neural-field-composer

1. Clone the repository.
2. Run the build.bat file.
3. Run the install.bat file.

### coppeliasim-cpp-client

https://github.com/Jgocunha/coppeliasim-cpp

1. Clone the repository.
2. Run the build.bat file.
3. Run the install.bat file.

### coppeliasim-vr-toolbox

https://github.com/BorisBogaerts/CoppeliaSim-VR-Toolbox

1. Download the latest release. CoppeliaSim VR Toolbox 2.2
2. Run the installer.
3. Got to the install directory 
    - Open the ```HTC_VIVE.lua``` file in the Scripts folder.
    - Comment line 40 and uncomment line 41
    ```lua
    40 --simpleShapeHandles=sim.ungroupShape(handles[i])
    41 simpleShapeHandles = {handles[i]}
    ```
    - Run ```copyToVrep.bat``` as admin.
4. To test you can open CoppeliaSim, open the Hello_vr_world.ttt scene, run the CoppeliaSim VR interface, and then run the scene. You should see the scene load to your vr headset.

## Up and running

1. Clone the project.
2. Open the CoppeliaSim scene ```scene.ttt```.
3. Run the build.bat file.
3. Run the ```vr-hr-joint-task-exe.ttt```.
4. Run the scene.