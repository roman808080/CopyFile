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

