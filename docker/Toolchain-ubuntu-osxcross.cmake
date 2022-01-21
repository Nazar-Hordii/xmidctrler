# Sample toolchain file for building for Mac OS X from an Ubuntu Linux system.
#
# Expected environment variables and example values:
# OSX_TOOLCHAIN_PREFIX = "x86_64-apple-darwin20.2"
# OSX_SDK_PATH = "/usr/osxcross/SDK/MacOSX11.1.sdk"

set(CMAKE_SYSTEM_NAME Darwin)
set(TOOLCHAIN_PREFIX $ENV{OSX_TOOLCHAIN_PREFIX})

# cross compilers to use for C and C++
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-clang)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-clang++-libc++)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)
add_compile_options(-mmacosx-version-min=10.15)
add_compile_definitions(GL_SILENCE_DEPRECATION=1)
add_link_options(-mmacosx-version-min=10.15)

# target environment on the build host system
#   set 1st to dir with the cross compiler's C/C++ headers/libs
set(CMAKE_FIND_ROOT_PATH $ENV{OSX_SDK_PATH})

# modify default behavior of FIND_XXX() commands to
# search for headers/libs in the target environment and
# search for programs in the build host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
