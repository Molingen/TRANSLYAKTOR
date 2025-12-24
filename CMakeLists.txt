cmake_minimum_required(VERSION 3.14)
project(ArithmeticTranslator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

add_executable(translator
        main.cpp
        translator.hpp
        stack.hpp
)

target_include_directories(translator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(translator_tests
        tests.cpp
)

target_include_directories(translator_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(translator_tests PRIVATE
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(translator_tests)

add_custom_target(run_tests
        COMMAND translator_tests
        DEPENDS translator_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)