LibDacron
#########

.. image:: https://travis-ci.org/opencv-ai/dacron.svg?branch=master
    :target: https://travis-ci.org/opencv-ai/dacron


Get the code::

  mkdir -p ~/code
  cd ~/code
  git clone https://github.com/opencv-ai/dacron.git

Build/Test with the docker
##########################

Uses the docker development environment from
https://github.com/opencv-ai/dev_env

First, install Docker
https://docs.docker.com/engine/installation/linux/ubuntulinux/.

Source ``path.bash.inc`` to get some development tools add to your path::

  . ~/code/dev_env/path.bash.inc
  mkdir -p ~/code/build_dacron && cd ~/code/build_dacron
  cv_cmake_init $HOME/code/dacron
  cv_make
  cv_test

Build/Test without docker
#########################

Assuming you have the dependencies installed::

   mkdir -p ~/code/build_dacron && cd ~/code/build_dacron
   cmake ~/code/dacron
   make
   ctest

Use dacron in your own cmake project
####################################

First install dacron (adjust ``CMAKE_INSTALL_PREFIX`` as desired)::

  cd ~/code/build_dacron
  make install

Then in your project's CMakeLists.txt::

  find_package(Dacron REQUIRED)
  include_directories(${Dacron_INCLUDE_DIRS})
  add_executable(foo foo.cc)
  target_link_libraries(foo ${Dacron_LIBRARIES})

Development
###########

Make sure to run the ``bootstrap.sh`` script, to setup githooks, that
check for whitespace and clang-format::

  ~/code/dacron/bootstrap.sh

Run what travis runs locally
-----------------------------

::

  cd ~/code/dacron
  ./cv_run ./travis_build.sh
