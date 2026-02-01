cmake_minimum_required(VERSION 3.25)

project(Playground)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_CLANG_TIDY clang-tidy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

include_directories("$ENV{VULKAN_SDK}/Include")
include_directories(${CMAKE_SOURCE_DIR}/src)

link_directories("$ENV{VULKAN_SDK}/Lib")

find_package(glfw3 CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenImageIO CONFIG REQUIRED)
find_package(SPIRV-Tools CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(unofficial-spirv-reflect CONFIG REQUIRED)
find_package(volk CONFIG REQUIRED)

# Function to collect all relevant files in a directory and create source groups
function(collect_sources_and_headers base_dir)
    file(GLOB_RECURSE sources ${base_dir}/*.cpp ${base_dir}/*.h)
    foreach(source IN LISTS sources)
        file(RELATIVE_PATH source_rel ${CMAKE_SOURCE_DIR} ${source})
        get_filename_component(source_path ${source_rel} PATH)
        string(REPLACE "/" "\\" source_group_name ${source_path})
        source_group(${source_group_name} FILES ${source})
        list(APPEND all_sources ${source})
    endforeach()
    set(${ARGV1} ${all_sources} PARENT_SCOPE)
endfunction()

#
# Playground SDK
#

collect_sources_and_headers(${CMAKE_SOURCE_DIR}/src SOURCES_SDK)
add_library(PlaygroundSDK ${SOURCES_SDK})
target_link_libraries(PlaygroundSDK PRIVATE volk::volk volk::volk_headers imgui::imgui glfw nlohmann_json::nlohmann_json OpenImageIO::OpenImageIO slang unofficial::spirv-reflect unofficial::shaderc::shaderc SPIRV-Tools-static)

# Workaround for OpenImageIO/vcpkg issue on Windows
if(WIN32)
    target_compile_options(PlaygroundSDK PRIVATE /utf-8)
endif()

#
# Example applications
#

collect_sources_and_headers(${CMAKE_SOURCE_DIR}/example01 SOURCES_EXAMPLE01)
add_executable(Example01 ${SOURCES_EXAMPLE01})
target_link_libraries(Example01 PRIVATE PlaygroundSDK volk::volk_headers unofficial::spirv-reflect)

#

collect_sources_and_headers(${CMAKE_SOURCE_DIR}/example02 SOURCES_EXAMPLE02)
add_executable(Example02 ${SOURCES_EXAMPLE02})
target_link_libraries(Example02 PRIVATE PlaygroundSDK volk::volk_headers unofficial::spirv-reflect)

#

collect_sources_and_headers(${CMAKE_SOURCE_DIR}/example03 SOURCES_EXAMPLE03)
add_executable(Example03 ${SOURCES_EXAMPLE03})
target_link_libraries(Example03 PRIVATE PlaygroundSDK volk::volk_headers unofficial::spirv-reflect)

#

collect_sources_and_headers(${CMAKE_SOURCE_DIR}/example04 SOURCES_EXAMPLE04)
add_executable(Example04 ${SOURCES_EXAMPLE04})
target_link_libraries(Example04 PRIVATE PlaygroundSDK volk::volk_headers unofficial::spirv-reflect)

#

collect_sources_and_headers(${CMAKE_SOURCE_DIR}/example05 SOURCES_EXAMPLE05)
add_executable(Example05 ${SOURCES_EXAMPLE05})
target_link_libraries(Example05 PRIVATE PlaygroundSDK volk::volk_headers unofficial::spirv-reflect)

collect_sources_and_headers(${CMAKE_SOURCE_DIR}/example06 SOURCES_EXAMPLE06)
add_executable(Example06 ${SOURCES_EXAMPLE06})
target_link_libraries(Example06 PRIVATE PlaygroundSDK volk::volk_headers unofficial::spirv-reflect)

#
# SDK tests
#

enable_testing()

file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/test/*.cpp ${CMAKE_SOURCE_DIR}/test/*_test.cpp)
add_executable(PlaygroundSDK_test ${TEST_SOURCES})
target_link_libraries(PlaygroundSDK_test PRIVATE GTest::gtest_main PlaygroundSDK volk::volk_headers unofficial::spirv-reflect)
