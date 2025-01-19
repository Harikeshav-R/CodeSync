cmake_minimum_required(VERSION 3.30)
project(CodeSync C)

set(CMAKE_C_STANDARD 23)

add_executable(CodeSync main.c argparse.c argparse.h
        commands.h
        repository.c
        repository.h
        utils.c
        utils.h
        commands.c)

# Specify the path to the libconfig headers and library
set(LIBCONFIG_INCLUDE_DIR "/opt/homebrew/Cellar/libconfig/1.7.3/include")
set(LIBCONFIG_LIBRARY "/opt/homebrew/Cellar/libconfig/1.7.3/lib/libconfig.dylib")

# Add the include directory
target_include_directories(CodeSync PRIVATE ${LIBCONFIG_INCLUDE_DIR})

# Link the libconfig library
target_link_libraries(CodeSync PRIVATE ${LIBCONFIG_LIBRARY})