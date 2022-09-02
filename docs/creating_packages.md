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