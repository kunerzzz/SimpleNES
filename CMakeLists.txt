cmake_minimum_required(VERSION 3.1)
project(SimpleNES)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(BUILD_STATIC FALSE CACHE STRING "Set this to link external libraries statically")

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS_PERF "${CMAKE_CXX_FLAGS_PERF} -O2 -pg -DENABLE_PERFORMANCE_REPORT")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -g")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

# Add sources
file(GLOB SOURCES
    "${PROJECT_SOURCE_DIR}/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# Copy keybindings.conf
file(COPY keybindings.conf DESTINATION .)

# Will add __FILENAME__ macros for all source files, which is the filename without full find_path
# Courtesy of SO
function(define_file_basename_for_sources targetname)
    get_target_property(source_files "${targetname}" SOURCES)
    foreach(sourcefile ${source_files})
        # Get source file's current list of compile definitions.
        get_property(defs SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS)
        # Add the FILE_BASENAME=filename compile definition to the list.
        get_filename_component(basename "${sourcefile}" NAME)
        list(APPEND defs "__FILENAME__=\"${basename}\"")
        # Set the updated compile definitions on the source file.
        set_property(
            SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS ${defs})
    endforeach()
endfunction()

# Specify include Directory
include_directories("${PROJECT_SOURCE_DIR}/include")

# Set static if BUILD_STATIC is set
if (BUILD_STATIC)
    # Link libgcc and libstc++ statically as well
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
    endif()
endif()

add_executable(SimpleNES ${SOURCES})

set_property(TARGET SimpleNES PROPERTY CXX_STANDARD 11)
set_property(TARGET SimpleNES PROPERTY CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(SimpleNES Threads::Threads)

target_link_libraries(SimpleNES)
define_file_basename_for_sources(SimpleNES)
