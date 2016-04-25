LibSplice
#########

.. image:: https://travis-ci.org/opencv-ai/splice.svg?branch=master
    :target: https://travis-ci.org/opencv-ai/splice

Build/Test with the docker
##########################

Uses the docker development environment from
https://github.com/opencv-ai/dev_env

First, install Docker
https://docs.docker.com/engine/installation/linux/ubuntulinux/.

Get the code::

  mkdir -p ~/code && cd ~/code && git clone https://github.com/opencv-ai/splice.git


Source ``path.bash.inc`` to get some development tools add to your path::

  . ~/code/dev_env/path.bash.inc
  mkdir -p ~/code/build_splice && cd ~/code/build_splice
  cv_cmake_init $HOME/code/splice
  cv_make
  cv_test

Build/Test without docker
#########################

  mkdir -p ~/code && cd ~/code && git clone https://github.com/opencv-ai/splice.git
  mkdir -p ~/code/build_splice && cd ~/code/build_splice
  cmake ~/code/splice
  make
  ctest

Use splice in your own cmake project
####################################

First install splice (adjust ``CMAKE_INSTALL_PREFIX`` as desired)::

  cd ~/code/build_splice
  make install

Then in your project's CMakeLists.txt::

  find_package(Splice REQUIRED)
  include_directories(${Splice_INCLUDE_DIRS})
  add_executable(foo foo.cc)
  target_link_libraries(foo ${Splice_LIBRARIES})

Development
###########

Make sure to run the ``bootstrap.sh`` script, to setup githooks, that
check for whitespace and clang-format::

  ~/code/splice/bootstrap.sh

Run what travis runs locally
-----------------------------

::

  cd ~/code/splice
  ./cv_run ./travis_build.sh
