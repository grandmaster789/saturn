cmake_minimum_required(VERSION 4.0)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(Saturn LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library   (Saturn)
add_executable(SaturnApp)
add_executable(SaturnTest)

find_package(Catch2 CONFIG REQUIRED)

file(GLOB_RECURSE SaturnLibSources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl"
)

file(GLOB_RECURSE SaturnAppSources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.inl"
)

file(GLOB_RECURSE SaturnTestSources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.inl"
)

target_sources(Saturn     PRIVATE ${SaturnLibSources})
target_sources(SaturnApp  PRIVATE ${SaturnAppSources})
target_sources(SaturnTest PRIVATE ${SaturnTestSources})

target_include_directories(SaturnApp  PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(SaturnTest PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(SaturnApp  PRIVATE Saturn)
target_link_libraries(SaturnTest PRIVATE
    Saturn
    Catch2::Catch2WithMain
)

if (MSVC)
    target_compile_definitions(Saturn     PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(SaturnTest PRIVATE _CRT_SECURE_NO_WARNINGS)

    target_compile_options(Saturn PRIVATE
        /W4 # enable all warnings
        /MP # enable multi-processor compilation
    )

    # this will avoid showing a console window upon startup
    #target_link_options(SaturnApp PRIVATE /SUBSYSTEM:WINDOWS /entry:mainCRTStartup)
else()
    # gcc/clang?
    target_compile_options(Saturn PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

enable_testing()
add_test(NAME SaturnUnitTests COMMAND SaturnTest)