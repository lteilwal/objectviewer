# CMakeLists.txt - build configuration for MeshViewer project
#
# what this file does:
# 1. sets up c++17 standard
# 2. locates cuda toolkit and nvcc compiler
# 3. compiles cuda kernel with native gpu architecture flags
# 4. builds MeshViewer executable and links sdl3 and cudart

cmake_minimum_required (VERSION 3.17)

project ("MeshViewer" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# locate cuda toolkit
find_package(CUDAToolkit REQUIRED)

# compile cuda kernel object file using nvcc with native architecture and matching runtime library
set(CUDA_KERNEL_OBJ "${CMAKE_CURRENT_BINARY_DIR}/kernelCalc.obj")
add_custom_command(
    OUTPUT ${CUDA_KERNEL_OBJ}
    COMMAND ${CUDAToolkit_NVCC_EXECUTABLE} -c "${CMAKE_CURRENT_SOURCE_DIR}/src/kernelCalc.cu"
            -I "${CMAKE_CURRENT_SOURCE_DIR}/inc"
            -o "${CUDA_KERNEL_OBJ}"
            -std=c++17
            -arch=native
            $<$<CONFIG:Debug>:-Xcompiler=/MDd>
            $<$<CONFIG:Release>:-Xcompiler=/MD>
            $<$<CONFIG:Debug>:-g>
            $<$<CONFIG:Release>:-O3>
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/kernelCalc.cu" 
            "${CMAKE_CURRENT_SOURCE_DIR}/inc/kernelCalc.h" 
            "${CMAKE_CURRENT_SOURCE_DIR}/inc/calc.h" 
            "${CMAKE_CURRENT_SOURCE_DIR}/inc/point.h"
    COMMENT "compiling cuda kernel kernelCalc.cu with native architecture"
)

add_subdirectory(ext/SDL EXCLUDE_FROM_ALL)

add_executable(MeshViewer 
    "src/main.cpp" 
    "src/win.cpp" 
    "src/mesh.cpp" 
    "src/input.cpp"
    "src/fileprocessor.cpp" 
    "src/calc.cpp" 
    "src/render.cpp" 
    ${CUDA_KERNEL_OBJ}
    "inc/point.h" 
    "inc/renderdata.h" 
    "inc/enum.h"
    "inc/kernelCalc.h"
)

target_include_directories(${PROJECT_NAME} PRIVATE inc ${CUDAToolkit_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 CUDA::cudart)