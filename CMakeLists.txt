cmake_minimum_required(VERSION 3.11)

project (WFC VERSION 1.0)

# compiler arguments
if (MSVC)
   add_compile_options(/Wall)
else()
   add_compile_options(-Wall -Wextra)
endif()

#specift C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenGl
find_package(OpenGL REQUIRED)

# Dependencies
find_package(raylib 4.0.0 QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
   include(FetchContent)
   FetchContent_Declare(
      raylib
      GIT_REPOSITORY   https://github.com/raysan5/raylib.git
      GIT_TAG          35c777ef2ccdad0b3a94b508ec13df5f6cd9ea49
   )
   FetchContent_GetProperties(raylib)
   if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
      set(FETCHCONTENT_QUIET NO)
      FetchContent_Populate(raylib)
      set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
      add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
   endif()
endif()

# enable interprocedural optimization if availible
if(ENABLE_IPO)
   include(CheckIPOSupported)
   check_ipo_supported( RESULT result OUTPUT output)
   if(result)
      set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
   else()
      message(SEND_ERROR "IPO is not supported: ${output}")
   endif()
endif()

# enable color in diagnortics
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
   add_compile_options(-fcolor-diagnostics)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
   add_compile_options(-fdiagnostics-color=always)
else()
   message(STATUS "No colored compiler diagnostic set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
endif()

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
endif()

# condfigure globals header to use absolute path to root dir
configure_file(src/globals.h.in ${PROJECT_SOURCE_DIR}/src/globals.h)

# set executable
add_executable(${PROJECT_NAME} src/main.cpp)

if (WIN32)
   set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "main")
else()
   set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "main.exe")
endif()

# link raylib
target_link_libraries(${PROJECT_NAME} raylib)

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()