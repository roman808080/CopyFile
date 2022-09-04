Creating packages with conan
============

[Main link to the creating packages documentation](https://docs.conan.io/en/latest/creating_packages.html)

[Mastering conan](https://docs.conan.io/en/latest/mastering.html)

[Create installer packages](https://docs.conan.io/en/latest/devtools/create_installer_packages.html#create-installer-packages)

[Running your server](https://docs.conan.io/en/latest/uploading_packages/running_your_server.html)

[Creating hello package](https://docs.conan.io/en/latest/creating_packages/getting_started.html)

Creating a new package:
~~~bash
conan new hello/0.1 --template=cmake_lib
~~~

Building the package:
~~~bash
conan create . demo/testing
~~~

How to list [local packages](https://docs.conan.io/en/latest/creating_packages/getting_started.html):
~~~bash
conan search
~~~


![image of locally installed packages](images/locally-installed-packages-hello-package.jpg "Here my packages with the hello world library")

Looking for the detail information with the next command:
~~~bash
conan search hello/0.1@demo/testing
~~~

![image of locally installed packages](images/looking-at-package-info.jpg "Detailed information")


> The `conan create` command receives the same command line parameters as conan install so you can pass to it the same settings and options. If we execute the following lines, we will create new package binaries for those configurations:

Creating a debug package

~~~bash
conan create . demo/testing -s build_type=Debug
~~~
>...
>
>hello/0.1: Hello World Debug!

Creating a release *.so packages:
~~~bash
conan create . demo/testing -o hello:shared=True
~~~

>...
>
>hello/0.1: Hello World Release!


![image of locally installed packages](images/the-list-of-package-options.jpg "Detailed information")



Cloning repositories
------------

Main link is [here](https://docs.conan.io/en/latest/creating_packages/external_repo.html).

In short:

1. By using `source()`

    ~~~python
    from conans import ConanFile, CMake, tools

    class HelloConan(ConanFile):
        ...

        def source(self):
            self.run("git clone https://github.com/conan-io/hello.git")
            ...
    ~~~

1. By using `source()` and [tools.Git](https://docs.conan.io/en/latest/reference/tools.html#tools-git)

    ~~~python
    from conans import ConanFile, CMake, tools

    class HelloConan(ConanFile):
        ...

        def source(self):
            git = tools.Git(folder="hello")
            git.clone("https://github.com/conan-io/hello.git", "master")
            ...
    ~~~

1. Using the [scm attribute](https://docs.conan.io/en/latest/reference/conanfile/attributes.html#scm-attribute)

    ~~~python
    from conans import ConanFile, CMake, tools

    class HelloConan(ConanFile):
        scm = {
            "type": "git",
            "subfolder": "hello",
            "url": "https://github.com/conan-io/hello.git",
            "revision": "master"
        }
        ...
    ~~~

Package Creation Process
------------

Original link is [here](https://docs.conan.io/en/latest/creating_packages/understand_packaging.html#package-creation-process).


![image of a conan package creation](images/conan-package_create_flow.png "Creation of conan package")

The citation from the documentation:

> Each package recipe contains five important folders in the local cache:
> * `export`: The folder in which the package recipe is stored.
> * `export_source`: The folder in which code copied with the recipe exports_sources attribute is stored.
> * `source`: The folder in which the source code for building from sources is stored.
> * `build`: The folder in which the actual compilation of sources is done. There will typically be one subfolder for each different binary configuration
> * `package`: The folder in which the final package artifacts are stored. There will be one subfolder for each different binary configuration


ABI Compatibility
------------
The link is [here](https://docs.conan.io/en/latest/creating_packages/define_abi_compatibility.html)
### It was skpped for now

Using Components
------------
Components allow define a few binaries inside one package (as *.a, *.so or just binaries).
By default it is advisable to have only one binary per package.

The link on documentation is [here](https://docs.conan.io/en/latest/creating_packages/package_information.html#using-components)


Packaging Approaches
------------
Documentation is [here](https://docs.conan.io/en/latest/creating_packages/package_approaches.html)
### Skip for now

Boost 1.80
------------

[One of the requested for boost 1.80](https://github.com/conan-io/conan-center-index/issues/12564)
[Pull request 1.80](https://github.com/conan-io/conan-center-index/pull/12807/files#diff-a1f0c325abfbf82c4e7d0df08fca1267cc98d0cf9376362813be62c7e68ff50c)

[Official patches](https://www.boost.org/patches/)

Patches in boost_1.79 ~/.conan/data/boost/1.79.0/_/_/source/conandata.yml
~~~yml
patches:
  1.79.0:
  - base_path: source_subfolder
    patch_file: patches/boost_locale_fail_on_missing_backend.patch
  - base_path: source_subfolder
    patch_file: patches/boost_1_77_mpi_check.patch
  - base_path: source_subfolder
    patch_file: patches/1.69.0-locale-no-system.patch
  - base_path: source_subfolder
    patch_file: patches/1.77.0-fiber-mingw.patch
  - base_path: source_subfolder
    patch_file: patches/1.79.0-0001-json-array-erase-relocate.patch
  - base_path: source_subfolder
    patch_file: patches/1.79.0-smart_ptr_cw_ppc_msync.patch
  - base_path: source_subfolder
    patch_file: patches/1.79.0-geometry_no_rtti.patch
    patch_source: https://github.com/boostorg/geometry/discussions/1041
    patch_type: portability
    url: https://github.com/boostorg/geometry/commit/b354162798749b3aaa539755e7b9be49d2b9a9c4.patch
sources:
  1.79.0:
    sha256: 475d589d51a7f8b3ba2ba4eda022b170e562ca3b760ee922c146b6c65856ef39
    url:
    - https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.tar.bz2
    - https://sourceforge.net/projects/boost/files/boost/1.79.0/boost_1_79_0.tar.bz2
~~~