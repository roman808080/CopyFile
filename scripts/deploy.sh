#/bin/env bash

# ./bootstrap.sh --prefix=/home/ubuntu/boost
# ./b2
# ./b2 install

SCRIPT_PATH=$(realpath $0)

SCRIPT_DIR=$(dirname $SCRIPT_PATH)
BASE_DIR=$(dirname $SCRIPT_DIR)
EXT_LIB_DIR=$BASE_DIR/ext
BOOST_LIB_DIR=$EXT_LIB_DIR/boost

mkdir -p $EXT_LIB_DIR
mkdir -p $BOOST_LIB_DIR

# to think about where to build and where to install
# possibly, it should be the same directory.
