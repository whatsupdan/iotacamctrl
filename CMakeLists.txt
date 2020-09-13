cmake_minimum_required(VERSION 3.0)
project(iotacamctrl LANGUAGES CXX)

find_package(PythonInterp 3.7 REQUIRED)
find_package(PythonLibs 3.7 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

set(PYBIND11_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/pybind11)
add_subdirectory(${PYBIND11_DIR})
pybind11_add_module(iotacamctrl ${SOURCES} "${SOURCE_DIR}/iotacamctrl.cpp")

### Append project cmake script dir ###
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "build")
endif()

### Enumerate project files ###
include(enum_cli_hdr)
include(enum_cli_src)
include(enum_crsdk_hdr)

### Define output target ###
# add_executable(iotacamctrl
#     ${cli_hdrs}
#     ${cli_srcs}
#     ${crsdk_hdrs}
# )

set_target_properties(
    iotacamctrl
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        BUILD_RPATH "$ORIGIN"
        INSTALL_RPATH "$ORIGIN"
)

set(SOURCE_DIR "src")
include_directories(${SOURCE_DIR})
set(SOURCES "${SOURCE_DIR}/iotacamctrl.cpp")

## Specify char is signed-char to fix mismatch with Raspbian
target_compile_options(iotacamctrl
    PRIVATE
        -fsigned-char
)

target_include_directories(iotacamctrl
    PRIVATE
        ${crsdk_hdr_dir} # defined in enum script
)

### Configure external library directories ###
set(ldir ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(cr_ldir ${ldir}/crsdk)

### Link CRSDK library
find_library(camera_remote Cr_Core HINTS ${cr_ldir})
target_link_libraries(iotacamctrl
    PRIVATE
        ${camera_remote}
)

### Windows specific configuration ###
if(WIN32)
    ## Build with unicode on Windows
    target_compile_definitions(iotacamctrl PRIVATE UNICODE _UNICODE)
endif(WIN32)

### Linux specific configuration ###
if(UNIX)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
            # Must use std::experimental namespace if older than GCC8
            message("[${PROJECT_NAME}] GCC version less than 8. Using std::experimental namespace.")
            target_compile_definitions(iotacamctrl PRIVATE USE_EXPERIMENTAL_FS)
        endif()

        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
            # Must explicitly link separate std::filesystem if older than GCC9
            message("[${PROJECT_NAME}] GCC version less than 9. Explicitly linking separate std::filesystem library.")
            target_link_libraries(iotacamctrl PRIVATE stdc++fs)
        endif()
    endif()
endif(UNIX)

## Copy required library binaries
add_custom_command(TARGET iotacamctrl PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${cr_ldir} "${PYTHON_LIBRARY_DIR}"
)


## Install application
## '.' means, install to the root directory of CMAKE_INSTALL_PREFIX
install(TARGETS iotacamctrl LIBRARY DESTINATION "${PYTHON_LIBRARY_DIR}")
install(DIRECTORY ${cr_ldir}/ DESTINATION "${PYTHON_LIBRARY_DIR}")
