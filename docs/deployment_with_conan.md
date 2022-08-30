Deployment with conan
============

Cmake (version 3.15 or above) and python3 (3.6 or above) are required.

Install virtual package
------------
To install virtualenv in python3 you need to input the next command:
~~~bash
pip install virtualenv
~~~

Here is an example how it looks in my environment:
![install virtualenv image](images/install_virtualenv.jpg "Install virtualenv for python3")

### If you have python2 as your default interpreter, you need to say explicitly to use python3.
~~~bash
pip3 install virtualenv
~~~

### If you do not have pip or pip3 you need to install it.
How to do this can be found [here](https://pip.pypa.io/en/stable/installation/).

Install virtual environment
------------
It is required to execute the next command to create a local python environment in the local directory:
~~~bash
virtualenv -p python local_python
~~~
or
~~~bash
virtualenv -p python3 local_python
~~~

You should see something like this:

![creation of virtualenv image](images/create_local_virtualenv.jpg "You need to see something like this")

The next command should be (if you are using bash):
~~~bash
source local_python/bin/activate
~~~

*However, if you use a different shell you need to source a different profile.*

For example, the next exampel sources a profile for [fish](https://fishshell.com/).
~~~bash
source local_python/bin/activate.fish
~~~

Here is an example how I source the fish profile:

![source the fish profile image](images/source_profile.jpg "Source the fish profile.")

The list of other profile can be found in *local_python/bin*:

![bin directory image](images/bin_directory.jpg "It is how the bin directory looks like.")

More information about virtual environments can be found [here](https://pythonbasics.org/virtualenv/).


Install Conan
------------

The next page about the [installation](https://docs.conan.io/en/latest/installation.html) process can be usefull.

However, basically, you need to run the next command:

~~~bash
pip install conan
~~~

Here how it looks in my environment:

![conan installation image](images/conan_installation.jpg "An example of output for conan installation.")


At the moment of the writing, conan2 is in beta and you can install it with the next command:
~~~bash
pip install conan --pre
~~~


Integration Conan with Cmake
------------

This [link](https://docs.conan.io/en/2.0/tutorial/consuming_packages/build_simple_cmake_project.html) was originally used to research the topic. However, it is worth to mention that it was written for conan2 which is in beta at the moment of writing.

Here we will use recipes which are present in the cloud (center.conan.io).
You can check current remotes with the next command:
~~~bash
conan remote list
~~~

For my environment it looks the next way:

![list of remotes image](images/list-of-remotes.jpg "Remotes which are present in my environment.")

This repository has a conan config file which is called *conanfile.txt*.
The content of the file is
~~~properties
[requires]
boost/1.79.0
gtest/1.12.1
cryptopp/8.6.0

[generators]
CMakeDeps
CMakeToolchain
~~~

This file has two sections:
* *\[requires\]* section is where we declare the libraries we want to use in the project, in this case, *boost/1.79.0*, *gtest/1.12.1* and *cryptopp/8.6.0*;
* *\[generators\]* section tells Conan to generate the files that the compilers or build systems will use to find the dependencies and build the project. In this case, as our project is based in CMake, we will use CMakeDeps to generate information about where the libraries files are installed and CMakeToolchain to pass build information to CMake using a CMake toolchain file.

