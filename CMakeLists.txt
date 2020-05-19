cmake_minimum_required(VERSION 3.5)

if($ENV{TRAVIS_BUILD_NUMBER})
    project(realty VERSION 0.1.$ENV{TRAVIS_BUILD_NUMBER} LANGUAGES CXX)
elseif($ENV{APPVEYOR_BUILD_NUMBER})
    project(realty VERSION 0.1.$ENV{APPVEYOR_BUILD_NUMBER} LANGUAGES CXX)
else()
    project(realty VERSION 0.1.1 LANGUAGES CXX)
endif()

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)
# End of gtest import

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(USE_AVX_INSTRUCTIONS ON)

find_package(dlib CONFIG REQUIRED)

configure_file(bintray.json.in bintray.json)

add_executable(rclst rclst.cpp)
target_link_libraries(rclst PRIVATE dlib::dlib)

add_executable(rclss rclss.cpp)
target_link_libraries(rclss PRIVATE dlib::dlib)

add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE gtest gtest_main)


install(TARGETS rclst DESTINATION bin)
install(TARGETS rclss DESTINATION bin)

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR "${realty_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${realty_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${realty_VERSION_PATCH}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alexey")
include(CPack)

enable_testing()
add_test(unit_tests tests)

if (MSVC)
    target_compile_options(
        rclst PRIVATE
        /W4
    )
    target_compile_options(
        rclss PRIVATE
        /W4
    )
    add_compile_definitions(_WIN32_WINNT=0x0601)
else ()
    target_compile_options(
        rclst PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(
        rclss PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()
