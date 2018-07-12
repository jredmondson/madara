#!/bin/sh

DIST_DIR=.

VERSION="$(cat $MADARA_ROOT/VERSION.txt)"
PACKAGE_NAME="madara"

LIB_VERSION="$(grep -oE '[0-9]+\.[0-9]+\.[0-9]+' $MADARA_ROOT/VERSION.txt)"
ARCHITECTURE=amd64
ROOT_DIR=usr
REVISION=1
INCLUDE_ALL_BIN=0
DEPENDS="build-essential (>= 12.1), subversion (>= 1.9.3-2), git-core (>= 1:2.7.4-0), perl (>= 5.22.1-9), libboost-all-dev (>= 1.58.0.1)"
DEPENDS_CLANG=0
DEPENDS_JAVA=0
DEPENDS_SSL=0
DEPENDS_ZEROMQ=0
DEPENDS_PYTHON=0
PRINT_USAGE=0

until [ -z "$1" ] ;
do
  if [ "$1" = "--help" -o "$1" = "-h" -o $PRINT_USAGE -eq 1 ] ;
  then
    echo "Packager options:"
    echo "  --lib-version VER    MADARA so version (def $VERSION)"
    echo "  --arch        ARCH   architecture type (e.g. i386, x64, all, etc.)"
    echo "  --root        DIR    root directory to install to (e.g. usr/local)"
    echo "  --revision    REV    revision number (1 by default)"
    echo "  --include-all-bin    include all files in $MADARA_ROOT/bin"
    echo "  --clang              package depends on clang"
    echo "  --java               package depends on java"
    echo "  --ssl                package depends on ssl"
    echo "  --zmq                package depends on zeromq"
    echo "  --python             package depends on python"
    exit 1
  elif [ "$1" = "--lib-version" ] ;
  then
    shift
    echo "Setting LIB_VERSION to $1"
    LIB_VERSION=$1
    shift
  elif [ "$1" = "--arch" ] ;
  then
    shift
    echo "Setting ARCHITECTURE to $1"
    ARCHITECTURE=$1
    shift
  elif [ "$1" = "--root" ] ;
  then
    shift
    echo "Setting ROOT_DIR to $1"
    ROOT_DIR=$1
    shift
  elif [ "$1" = "--revision" ] ;
  then
    shift
    echo "Setting REVISION to $1"
    REVISION=$1
    shift
  elif [ "$1" = "--include-all-bin" ] ;
  then
    echo "Including all bins in MADARA_ROOT/bin"
    INCLUDE_ALL_BIN=1
    shift
  elif [ "$1" = "--clang" ] ;
  then
    echo "Marking clang dependencies"
    DEPENDS="$DEPENDS, clang-5.0 (>= 1:5.0-3~16.04.1), libc++-dev (>= 3.7.0-1), libc++abi-dev (>= 3.7.0-1)"
    DEPENDS_CLANG=1
    shift
  elif [ "$1" = "--java" ] ;
  then
    echo "Marking java dependencies"
    DEPENDS="$DEPENDS, oracle-java8-set-default (>= 8u171-1~webupd8~0), maven (>= 3.3.9-3)"
    DEPENDS_JAVA=1
    shift
  elif [ "$1" = "--ssl" ] ;
  then
    echo "Marking ssl dependencies"
    DEPENDS="$DEPENDS, libssl-dev (>= 1.0.2g-1ubuntu4.13)"
    DEPENDS_SSL=1
    shift
  elif [ "$1" = "--zmq" ] ;
  then
    echo "Marking zeromq dependencies"
    DEPENDS_ZEROMQ=1
    shift
  elif [ "$1" = "--python" ] ;
  then
    echo "Marking python dependencies"
    DEPENDS="$DEPENDS, python (>= 2.7.12-1~16.04), python2.7 (>= 2.7.12-1ubuntu0~16.04.3)"
    DEPENDS_PYTHON=1
    shift
  else
    PRINT_USAGE=1
    shift;
  fi

done

sudo apt-get install dpkg-dev dh-make debhelper devscripts pbuilder fakeroot dos2unix

echo "VERSION is $VERSION"
echo "ARCHITECTURE is $ARCHITECTURE"
echo "Generating debian packaging into $DIST_DIR"

# copy source to local directories
cd $DIST_DIR

PACKAGE_DIR="${VERSION}-${REVISION}_${ARCHITECTURE}"

if [ $DEPENDS_CLANG -eq 1 ] ; then
  PACKAGE_NAME="${PACKAGE_NAME}-clang"
fi

if [ $DEPENDS_JAVA -eq 1 ] ; then
  PACKAGE_NAME="${PACKAGE_NAME}-java"
fi

if [ $DEPENDS_PYTHON -eq 1 ] ; then
  PACKAGE_NAME="${PACKAGE_NAME}-python"
fi

if [ $DEPENDS_SSL -eq 1 ] ; then
  PACKAGE_NAME="${PACKAGE_NAME}-ssl"
fi

if [ $DEPENDS_ZEROMQ -eq 1 ] ; then
  PACKAGE_NAME="${PACKAGE_NAME}-zmq"
fi

PACKAGE_DIR="${PACKAGE_NAME}_${PACKAGE_DIR}"

# create a source tarball for ACE and MADARA source
echo "Creating madara-$VERSION directory"
sudo rm -rf $PACKAGE_DIR
mkdir $PACKAGE_DIR
cd $PACKAGE_DIR

# we'll use the dpkg-deb script to read our debian directory, and
# dpkg-deb requires a DEBIAN folder and not the debian that dh does

mkdir DEBIAN

# create needed directories 
echo "Creating directories for source code and libraries"
mkdir -p $ROOT_DIR/include
mkdir -p $ROOT_DIR/lib
mkdir -p $ROOT_DIR/bin
mkdir -p $ROOT_DIR/doc/madara

# we want to add the new LD_LIBRARY_PATH
echo "Updating ld.so.conf.d"
mkdir -p etc/ld.so.conf.d
echo /$ROOT_DIR/lib >> etc/ld.so.conf.d/madara.conf

if [ $DEPENDS_ZEROMQ -eq 1 ] ; then
  cp /usr/local/lib/libzmq.so* $ROOT_DIR/lib/
  cp /usr/local/include/zmq* $ROOT_DIR/include/
fi

# copy the MADARA source includes into the appropriate directory
echo "Copying MADARA source trees for developers"
cp -r $MADARA_ROOT/include/madara $ROOT_DIR/include/

# remove any shared objects, symbolic links and svn from the include directory
echo "Removing all symbolic links from source tree"
find $ROOT_DIR/include -type l -exec rm -f {} \;
echo "Removing all shared objects from source tree"
find $ROOT_DIR/include -name "*.so*" -type f -exec rm -f {} \;
echo "Removing all shared objects from source tree"
find $ROOT_DIR/include -name "*.o*" -type f -exec rm -f {} \;
echo "Removing all git directories from source tree"
find $ROOT_DIR/include -name .git -type d -exec rm -rf {} 2> /dev/null \;

# update changelog with the last 10 entries from the repository
echo "Saving last 10 git changelog entries into debian changelog"
git -C $MADARA_ROOT log --no-merges -10 > $ROOT_DIR/doc/madara/changelog

# merge MADARA copyright and license information
cat $MADARA_ROOT/LICENSE.txt >> $ROOT_DIR/doc/madara/copyright
dos2unix $ROOT_DIR/doc/madara/copyright

# we recently stopped using named sos, so we only need to copy over libMADARA
#cp $MADARA_ROOT/libMADARA.so.$LIB_VERSION $ROOT_DIR/lib
echo "Copying libMADARA.so to $ROOT_DIR/lib"
cp $MADARA_ROOT/libMADARA.so $ROOT_DIR/lib

if [ $DEPENDS_PYTHON -eq 1 ] ; then
  echo "Copying Python madara.so to $ROOT_DIR/lib"
  cp $MADARA_ROOT/lib/madara.so $ROOT_DIR/lib
fi

# 
echo "Copying binaries to $ROOT_DIR/bin"
if [ $INCLUDE_ALL_BIN -eq 0 ] ; then
  cp $MADARA_ROOT/bin/madara_version $ROOT_DIR/bin/madara_version
  cp $MADARA_ROOT/bin/karl $ROOT_DIR/bin/karl
else
  cp $MADARA_ROOT/bin/* $ROOT_DIR/bin/
fi

OLD_DIR=$(pwd)

cd $ROOT_DIR/lib


#ln -s libMADARA.so.$LIB_VERSION libMADARA.so 

cd $OLD_DIR

sudo chown -R root.root $ROOT_DIR

# modify the control file to be specific to MADARA
echo "Package: ${PACKAGE_NAME}" >> DEBIAN/control
echo "Priority: extra" >> DEBIAN/control
echo "Section: libs" >> DEBIAN/control
echo "Architecture: $ARCHITECTURE" >> DEBIAN/control
echo "Maintainer: James Edmondson <jedmondson@gmail.com>" >> DEBIAN/control 
echo "Version: $VERSION" >> DEBIAN/control
echo "Depends: $DEPENDS" >> DEBIAN/control
#echo "Depends: debhelper (>= 8.0.0), libdl (>=2), librt (>=1), libstdc++ (>=6), libgcc (>=1), libpthread (>=0), libc (>=6), ld-linux (>=2)" >> DEBIAN/control
echo "Homepage: http:\/\/madara.ai" >> DEBIAN/control
echo "Description: Libraries for the MADARA middleware, version $VERSION" >> DEBIAN/control 



# create the debian package
cd ..
dpkg-deb -b $PACKAGE_DIR

exit 0
