#!/bin/sh

DIST_DIR=.

VERSION="$(cat $MADARA_ROOT/VERSION.txt)"

# because of silliness with the gnuace type for MPC, our MADARA
# libs are not actually created with the correct version. Instead,
# MPC creates a shared object with the ACE_VERSION + 1 minor version
# The following variables MUST BE CHANGED with each new ACE version used
ACE_VERSION="$(grep -oE '[0-9]+\.[0-9]+\.[0-9]+' $ACE_ROOT/VERSION)"
LIB_VERSION=$ACE_VERSION
ARCHITECTURE=amd64
ROOT_DIR=usr/local
REVISION=1

until [ -z "$1" ] ;
do
  if [ "$1" = "--help" -o "$1" = "-h" ] ;
  then
    echo "Packager options:"
    echo "  --ace-version VER    indicates the ace version (e.g. $ACE_VERSION)"
    echo "  --lib-version VER    MADARA so version (reflection of ACE version)"
    echo "                       This is not the contents of VERSION.txt"
    echo "                       See $MADARA_ROOT/lib/libMADARA.so.(VER)"
    echo "  --arch        ARCH   architecture type (e.g. i386, x64, all, etc.)"
    echo "  --root        DIR    root directory to install to (e.g. usr/local)"
    echo "  --revision    REV    revision number (1 by default)"
    exit 1
  elif [ "$1" = "--ace-version" ] ;
  then
    shift
    echo "Setting ACE_VERSION to $1"
    ACE_VERSION=$1
    shift
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
  fi

done

sudo apt-get install dpkg-dev dh-make debhelper devscripts pbuilder fakeroot dos2unix

echo "VERSION is $VERSION"
echo "ARCHITECTURE is $ARCHITECTURE"
echo "Generating debian packaging into $DIST_DIR"

# copy source to local directories
cd $DIST_DIR

PACKAGE_DIR="madara_${VERSION}-${REVISION}_${ARCHITECTURE}"

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

# copy the MADARA and ACE source includes into the appropriate directory
echo "Copying MADARA and ACE source trees for developers"
cp -r $MADARA_ROOT/include/madara $ROOT_DIR/include/
cp -r $ACE_ROOT/ace $ROOT_DIR/include/

# remove any shared objects, symbolic links and svn from the include directory
echo "Removing all symbolic links from source tree"
find $ROOT_DIR/include -type l -exec rm -f {} \;
echo "Removing all shared objects from source tree"
find $ROOT_DIR/include -name "*.so*" -type f -exec rm -f {} \;
echo "Removing all SVN directories from source tree"
find $ROOT_DIR/include -name .svn -type d -exec rm -rf {} 2> /dev/null \;

# update changelog with the last 10 entries from the repository
echo "Saving last 10 svn changelog entries into debian changelog"
svn log http://madara.googlecode.com/svn/trunk/ -l 10 > $ROOT_DIR/doc/madara/changelog

# merge MADARA and ACE copyright and license information
cat $MADARA_ROOT/LICENSE.txt >> $ROOT_DIR/doc/madara/copyright
cat $ACE_ROOT/COPYING >> $ROOT_DIR/doc/madara/copyright
dos2unix $ROOT_DIR/doc/madara/copyright

cp $ACE_ROOT/ace/libACE.so.$ACE_VERSION $ROOT_DIR/lib
cp $MADARA_ROOT/libMADARA.so.$LIB_VERSION $ROOT_DIR/lib

cp $MADARA_ROOT/bin/system_calls $ROOT_DIR/bin/madara_system_calls
cp $MADARA_ROOT/bin/madara_version $ROOT_DIR/bin/madara_version


OLD_DIR=$(pwd)

cd $ROOT_DIR/lib


ln -s libACE.so.$ACE_VERSION libACE.so
ln -s libMADARA.so.$LIB_VERSION libMADARA.so 

cd $OLD_DIR

sudo chown -R root.root $ROOT_DIR

# modify the control file to be specific to MADARA
echo "Package: madara" >> DEBIAN/control
echo "Priority: extra" >> DEBIAN/control
echo "Section: libs" >> DEBIAN/control
echo "Architecture: $ARCHITECTURE" >> DEBIAN/control
echo "Maintainer: James Edmondson <jedmondson@gmail.com>" >> DEBIAN/control 
echo "Version: $VERSION" >> DEBIAN/control
echo "Depends: " >> DEBIAN/control
#echo "Depends: debhelper (>= 8.0.0), libdl (>=2), librt (>=1), libstdc++ (>=6), libgcc (>=1), libpthread (>=0), libc (>=6), ld-linux (>=2)" >> DEBIAN/control
echo "Homepage: http:\/\/madara.googlecode.com" >> DEBIAN/control
echo "Description: Libraries for the MADARA middleware, version $VERSION" >> DEBIAN/control 



# create the debian package
cd ..
dpkg-deb -b $PACKAGE_DIR

