cmake_minimum_required(VERSION 3.2)

# Default build-time configuration options
# Can be modified via CMake GUI or via CMake command line
option(SSE42_POPCNT "Use POPCNT CPU in SSE4.2 instruction" ON)
option(HAMT_TEST_USE_DLMALLOC "Use DLMalloc instead of the default C runtime platform malloc" ON)
option(WIN64 "Default generate x64" ON)

# Write build-time configuration options to a header file
configure_file(config.h.in config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(.)
include_directories(../include)
include_directories(./random)
include(../cmake/BuildSettings.cmake)

# Maps to a solution file (Tutorial.sln). The solution will 
# have all targets (exe, lib, dll) as projects (.vcproj)
project(RWLockTest)

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Create project and .exe
# Debug;Release;MinSizeRel;RelWithDebInfo
if (CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
    set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
        "Reset the configurations to what we need"
        FORCE)
endif()

# Command to output information to the console
# Useful for displaying errors, warnings, and debugging
message("cxx Flags: " ${CMAKE_CXX_FLAGS})

file(GLOB SRCFILES *.cpp)
file(GLOB INCFILES *.h)

file(GLOB RANDOM_FILES "random/*.cpp", "random/*.h")

# Create named folders for the sources within the .vcproj
# Empty name lists them directly under the .vcproj
source_group("random" FILES ${RANDOM_FILES})

add_executable(${PROJECT_NAME}
    ${SRCFILES} ${INCFILES}
    config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h
    ${RANDOM_FILES})

# Add subdirectories with dependencies before dependents so that each subdirectory
# inherits the include paths gathered from the dependencies before it.
add_subdirectory(../src RWLock)

# Add libraries with dependencies after dependents to satisfy ld linker.
target_link_libraries(${PROJECT_NAME} RWLock)

# Adds logic to INSTALL.vcproj to copy RWLockTest.exe to destination directory
install(TARGETS RWLockTest
		 RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)