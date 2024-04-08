cmake_minimum_required(VERSION 3.15)

project (vr-hr-joint-task
    DESCRIPTION "A C++ application that runs a human robot joint task experiment in VR."
	LANGUAGES CXX
)

# Define library version
set(HR_VR_PROJ_VERSION_MAJOR 1 CACHE STRING "major version" FORCE)
set(HR_VR_PROJ_VERSION_MINOR 0 CACHE STRING "minor version" FORCE)
set(HR_VR_PROJ_VERSION ${HR_VR_PROJ_VERSION_MAJOR}.${HR_VR_PROJ_VERSION_MINOR} CACHE STRING "version" FORCE)

set(HR_VR_PROJ_LIB_PREFIX "release")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(HR_VR_PROJ_LIB_PREFIX "debug")
endif()

set(HR_VR_PROJ_INC_INSTALL_DIR "include")
set(HR_VR_PROJ_RUNTIME_INSTALL_DIR "bin")
set(HR_VR_PROJ_LIBRARY_INSTALL_DIR "bin")
set(HR_VR_PROJ_ARCHIVE_INSTALL_DIR "lib")
set(HR_VR_PROJ_FRAMEWORK_INSTALL_DIR "bin")
set(HR_VR_PROJ_CMAKE_CONFIG_INSTALL_DIR "share/${CMAKE_PROJECT_NAME}/cmake")
set(HR_VR_PROJ_ADDITIONAL_FILES_INSTALL_DIR "share/${CMAKE_PROJECT_NAME}")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check whether VCPKG is set up in your system
if(NOT DEFINED ENV{VCPKG_ROOT})
  message(FATAL_ERROR "ERROR: This project requires VCPKG.\n")
endif()

# Set VCPKG root directory
set(VCPKG_ROOT $ENV{VCPKG_ROOT})

# Include VCPKG toolchain
include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

# Set the project directory
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}")
# Pass the PROJECT_DIR as a preprocessor definition
add_compile_definitions(PROJECT_DIR="${PROJECT_DIR}")

# Set the desired data output directory
set(OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/data")
# Pass the OUTPUT_DIRECTORY as a preprocessor definition
add_compile_definitions(OUTPUT_DIRECTORY="${OUTPUT_DIRECTORY}")

# Set header files
set(header
    "include/experiment.h"
    "include/dnf_architecture.h"
    "include/misc.h"
    "include/dnf_composer_handler.h"
    "include/coppeliasim_handler.h"
    "include/event_logger.h"
)

# Set source files
set(src 
    "src/experiment.cpp"
    "src/dnf_architecture.cpp"
    "src/misc.cpp"
    "src/dnf_composer_handler.cpp"
    "src/coppeliasim_handler.cpp"
    "src/event_logger.cpp"
)

configure_file(./resources/resources.rc.in ./resources/resources.rc)

# Define library target
add_library(${CMAKE_PROJECT_NAME} ${header} ${src} ./resources/resources.rc)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> 
    $<INSTALL_INTERFACE:${HR_VR_PROJ_INC_INSTALL_DIR}> 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Setup imgui
find_package(imgui CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE imgui::imgui "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")

# Setup implot
find_package(implot CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE implot::implot)

# Setup nlohmann-json
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

# Setup dynamic-neural-field-composer
find_package(dynamic-neural-field-composer REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE dynamic-neural-field-composer)

# Setup coppeliasim-cpp-client
find_package(coppeliasim-cpp-client REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE coppeliasim-cpp-client)

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
                            HR_VR_PROJ=1
                            HR_VR_PROJ_VERSION_MAJOR=${HR_VR_PROJ_VERSION_MAJOR}
                            HR_VR_PROJ_VERSION_MINOR=${HR_VR_PROJ_VERSION_MINOR}
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
                                       OUTPUT_NAME "${CMAKE_PROJECT_NAME}-${HR_VR_PROJ_VERSION}"
                                       POSITION_INDEPENDENT_CODE ON
)

install(TARGETS ${CMAKE_PROJECT_NAME} EXPORT ${CMAKE_PROJECT_NAME}Targets
        RUNTIME       DESTINATION ${HR_VR_PROJ_RUNTIME_INSTALL_DIR}
        LIBRARY       DESTINATION ${HR_VR_PROJ_LIBRARY_INSTALL_DIR}
        ARCHIVE       DESTINATION ${HR_VR_PROJ_ARCHIVE_INSTALL_DIR}
        FRAMEWORK     DESTINATION ${HR_VR_PROJ_FRAMEWORK_INSTALL_DIR}
)

include(CMakePackageConfigHelpers)
set(version_file "${CMAKE_CURRENT_BINARY_DIR}/cmake/${CMAKE_PROJECT_NAME}-config-version.cmake")
write_basic_package_version_file(${version_file}
                                 VERSION ${HR_VR_PROJ_VERSION}
                                 COMPATIBILITY AnyNewerVersion
)

install(FILES ${header} DESTINATION ${HR_VR_PROJ_INC_INSTALL_DIR})

install(EXPORT ${CMAKE_PROJECT_NAME}Targets DESTINATION ${HR_VR_PROJ_CMAKE_CONFIG_INSTALL_DIR}
                                       FILE ${CMAKE_PROJECT_NAME}-config.cmake
)

# Add executable project
set(EXE_PROJECT ${CMAKE_PROJECT_NAME}-exe)
add_executable(${EXE_PROJECT} "src/main.cpp" ./resources/resources.rc)
target_include_directories(${EXE_PROJECT} PRIVATE include)
target_link_libraries(${EXE_PROJECT} PRIVATE imgui::imgui ${CMAKE_PROJECT_NAME})
target_link_libraries(${EXE_PROJECT} PRIVATE dynamic-neural-field-composer)
target_link_libraries(${EXE_PROJECT} PRIVATE coppeliasim-cpp-client)


# Setup Catch2
enable_testing()
find_package(Catch2 CONFIG REQUIRED)
include(CTest)
include(Catch)

# Add test project
set(TEST_PROJECT ${CMAKE_PROJECT_NAME}-test)
add_executable(${TEST_PROJECT} 
    tests/test.cpp 
)
target_include_directories(${TEST_PROJECT} PRIVATE include)
target_link_libraries(${TEST_PROJECT} PRIVATE 
	Catch2::Catch2 
	Catch2::Catch2WithMain 
	${CMAKE_PROJECT_NAME} 
	dynamic-neural-field-composer
	coppeliasim-cpp-client
)

# Automatically discover and add tests
catch_discover_tests(${TEST_PROJECT})