cmake_minimum_required(VERSION 3.0)
project(iotacamctrl VERSION 0.0.1)

### Append project cmake script dir ###
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

### Enumerate project files ###
include(enum_cli_hdr)
include(enum_cli_src)
include(enum_crsdk_hdr)


set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOURCES ${SOURCE_DIR}/iotacamctrl.cpp)



set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# include_directories(${INCLUDE_DIR})

# add_library(MyExternalLib SHARED IMPORTED)


add_subdirectory(${LIB_DIR}/pybind11)

pybind11_add_module(iotacamctrl SHARED  ${cli_hdrs} ${cli_srcs} ${crsdk_hdrs} ${SOURCES})

set_target_properties(
    iotacamctrl
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)




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
set(cr_ldir ${LIB_DIR}/crsdk)

### Link CRSDK library
find_library(camera_remote Cr_Core HINTS ${cr_ldir})
target_link_libraries(iotacamctrl
    PRIVATE
        ${camera_remote}
)







