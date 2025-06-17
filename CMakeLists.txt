cmake_minimum_required(VERSION 3.13)

project(RedisLite)

file(GLOB_RECURSE SOURCE_FILES src/*.c include/*.h)

set(CMAKE_C_STANDARD 23) # Enable the C23 standard

# Default to Debug if no build type specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

add_executable(redislite ${SOURCE_FILES}) # Basically does this gcc -o redislite [all .c files from SOURCE_FILES]

target_include_directories(redislite PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

install(TARGETS redislite
    RUNTIME DESTINATION bin
)