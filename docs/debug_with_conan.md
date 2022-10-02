Debug with conan
============

Release commands
------------

Creating build directory:
~~~bash
mkdir build
cd build
~~~

Release profile (profile for release is `copy_file_profile`):
~~~properties
[settings]
os=Linux
os_build=Linux
arch=armv8
arch_build=armv8
compiler=gcc
compiler.version=11
compiler.libcxx=libstdc++11
build_type=Release
[options]
[build_requires]
[env]
~~~

Command to generate files with conan for release:
~~~bash
conan install .. --build=missing -pr:b copy_file_profile -pr:h copy_file_profile
~~~

Command for cmake:
~~~bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
~~~


Debug commands
------------

Creating build directory:
~~~bash
mkdir build
cd build
~~~

Debug profile (profile is `copy_file_debug_profile`):
~~~properties
[settings]
os=Linux
os_build=Linux
arch=armv8
arch_build=armv8
compiler=gcc
compiler.version=11
compiler.libcxx=libstdc++11
build_type=Debug
[options]
[build_requires]
[env]
~~~

Command to generate files with conan for release:
~~~bash
conan install .. --build=missing -pr:b  copy_file_debug_profile -pr:h copy_file_debug_profile
~~~

Command for cmake:
~~~bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
~~~

