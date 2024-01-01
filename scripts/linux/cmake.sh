#!/bin/bash

SCRIPT_PATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

MADARA_PATH=$SCRIPT_PATH/../..

C_COMPILER='gcc'
CPP_COMPILER='g++'
BUILD_TYPE='Release'
TESTS='OFF'
SSL='OFF'
CLEAN=0
PREREQS=0
MAC=0

echo "Parsing args..."
for var in "$@"
do
  if [ "$var" = "clang" ] || [ "$var" = "CLANG" ]; then
    echo "  Using clang for compilation..."
    C_COMPILER='clang'
    CPP_COMPILER='clang++'
  elif [ "$var" = "clean" ] || [ "$var" = "CLEAN" ]; then
    echo "  Cleaning build..."
    CLEAN=1
  elif [ "$var" = "gcc" ] || [ "$var" = "g++" ]; then
    echo "  Using g++ for compilation..."
    C_COMPILER='gcc'
    CPP_COMPILER='g++'
  elif [ "$var" = "mac" ] || [ "$var" = "MAC" ]; then
    echo "  Configuring for MacOS..."
    MAC=1
  elif [ "$var" = "prereqs" ] || [ "$var" = "PREREQS" ]; then
    echo "  Installing prereqs..."
    PREREQS=1
  elif [ "$var" = "tests" ] || [ "$var" = "TESTS" ]; then
    echo "  Enabling tests..."
    TESTS='ON'
  elif [ "$var" = "ssl" ] ||  [ "$var" = "SSL" ]; then
    echo "  Enabling SSL support..."
    SSL='ON'
  else
#    echo "Invalid argument: $var"
    echo ""
    echo "Args can be zero or more of the following, space delimited"
    echo ""
    echo "  prereqs         install prereqs"
    echo "  clang           build using clang++ and clang"
    echo "  clean           run 'make clean' before builds (default)"
    echo "  docs            generate API documentation"
    echo "  gcc             use gcc/g++"
    echo "  tests           build test executables"
    echo "  ssl             build with SSL support"
    echo ""
    exit
  fi
done


echo SCRIPT_PATH=$SCRIPT_PATH
echo MADARA_PATH=$MADARA_PATH

echo "Creating build and install directories..."
cd $MADARA_PATH

if [ $CLEAN -eq 1 ]; then
  echo "Cleaning build directory..."
  rm -rf build
fi

if [ $MAC -eq 1 ]; then
  echo "Mac detected: setting compiler to clang..."
  C_COMPILER='clang'
  CPP_COMPILER='clang++'
fi

mkdir build
mkdir install
cd build

if [ $PREREQS -eq 1 ]; then
  echo "Installing prerequisites..."
  if [ $MAC -eq 1 ]; then
    echo "  Installing MacOS prerequisites..."
    brew install boost autoconf automake libtool git openssl
    brew link --force openssl
  else
    echo "  Installing Linux prerequisites..."
    sudo apt-get install -y build-essential libboost-all-dev git libssl-dev clang
  fi
fi

echo cmake -DCMAKE_CXX_COMPILER=${CPP_COMPILER} -DCMAKE_C_COMPILER=${C_COMPILER} -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -Dmadara_TESTS=${TESTS} -Dmadara_SSL=${SSL} -D'CMAKE_INSTALL_PREFIX=/usr/local' -DCMAKE_PREFIX_PATH=$MADARA_PATH/install ..
cmake -D'CMAKE_INSTALL_PREFIX=/usr/local' -Dmadara_TESTS=$TESTS -Dmadara_SSL=$SSL -DCMAKE_PREFIX_PATH=$MADARA_PATH/install -DCMAKE_CXX_COMPILER=$CPP_COMPILER -DCMAKE_C_COMPILER=$C_COMPILER -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
cmake --build . --config debug -j$(nproc)
cmake --build . --config release
sudo cmake --build . --target install --config release
sudo cmake --build . --target install --config debug
sudo ldconfig
