# Anticipatory Action Selection in Human-Robot Interaction

A virtual reality experiment exploring the impact of anticipatory behaviors in robots on joint task performance and human experience during collaborative pick-and-place tasks.

## Project Overview

This project implements a cognitive control system using dynamic neural field (DNF) architecture that enables a robot to predict and adapt to human actions in real-time. The system was evaluated in a virtual reality environment where participants completed a collaborative pick-and-place task with the robot under two conditions: anticipatory and non-anticipatory robot behavior.

### Key Findings

- Anticipatory mechanisms significantly improved subjective measures of robot perception
- Participants rated the anticipatory robot higher in competence and warmth
- Lower discomfort was reported when anticipation was enabled
- Significant reductions in collision frequency with anticipatory behavior
- Most participants (9/10) preferred collaborating with the anticipatory robot

## Setup Requirements

### Hardware
- HTC VIVE or HTC VIVE Pro Eye VR headset (both tested and working)
- Controllers for interaction in VR environment

### Software Dependencies

#### Required Tools
1. **CMake**
   - Download and install from [cmake.org](https://cmake.org/)

2. **vcpkg**
   ```bash
   git clone https://github.com/microsoft/vcpkg
   ./vcpkg/bootstrap-vcpkg.bat
   ```
   - Set the `VCPKG_ROOT` environment variable to point to your vcpkg directory

#### Required Libraries

1. **Dynamic Neural Field Composer**
   - Implementation of the cognitive architecture based on dynamic neural fields
   - Repository: [github.com/Jgocunha/dynamic-neural-field-composer](https://github.com/Jgocunha/dynamic-neural-field-composer)
   - Installation:
     1. Clone the repository
     2. Run `build.bat`
     3. Run `install.bat`

2. **CoppeliaSim C++ Client**
   - Interface for communication with the CoppeliaSim robotics simulator
   - Repository: [github.com/Jgocunha/coppeliasim-cpp](https://github.com/Jgocunha/coppeliasim-cpp)
   - Installation:
     1. Clone the repository
     2. Run `build.bat`
     3. Run `install.bat`

3. **CoppeliaSim VR Toolbox**
   - Enables integration with VR headsets
   - Repository: [github.com/BorisBogaerts/CoppeliaSim-VR-Toolbox](https://github.com/BorisBogaerts/CoppeliaSim-VR-Toolbox)
   - Installation:
     1. Download the latest release (CoppeliaSim VR Toolbox 2.2)
     2. Run the installer
     3. Go to the installation directory
     4. Open the `HTC_VIVE.lua` file in the Scripts folder
     5. Comment line 40 and uncomment line 41:
        ```lua
        40 --simpleShapeHandles=sim.ungroupShape(handles[i])
        41 simpleShapeHandles = {handles[i]}
        ```
     6. Run `copyToVrep.bat` as administrator

#### Optional Libraries (Installed Automatically by build.bat)
- imgui with docking-experimental, core, dx12-binding, win32-binding (x64-windows)
- implot (x64-windows)
- nlohmann-json (x64-windows)

## Getting Started

1. Clone this repository
2. Open the CoppeliaSim scene `scene.ttt`
3. Run the `build.bat` file to compile the project
4. Launch the `vr-hr-joint-task-exe` executable
5. Run the scene in CoppeliaSim
6. Put on the VR headset to begin the experiment

## Experiment Design

The experiment employs a within-subjects design with two conditions:
- **Anticipation-on**: Robot exhibits anticipatory behavior by predicting the participant's next action
- **Anticipation-off**: Robot selects objects arbitrarily with no predictive behavior

In each trial, the participant and robot collaboratively place three objects into a container.

## Cognitive Architecture

The project uses a dynamic neural field (DNF) architecture with four primary fields:
- **Action Observation Layer (AOL)**: Encodes the trajectory of the human hand
- **Object Representation Layer (ORL)**: Encodes objects in the workspace
- **Action Simulation Layer (ASL)**: Represents the robot's internal understanding of human intentions
- **Action Execution Layer (AEL)**: Encodes the robot's target actions

## Troubleshooting

- VR Toolbox may crash on first launch but typically works fine afterward
- Dynamic loading of objects is not currently supported once simulation has started
- If experiencing issues with grouped models in VR:
  1. Make the lua file adjustment described in the installation instructions, or
  2. Run the simulation, stop it, press Ctrl+Z to undo when you see broken models, then save (Ctrl+S)

## Citation

If you use this project in your research, please cite:
```
missing
```

