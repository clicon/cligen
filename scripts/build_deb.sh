#!/bin/sh

set -e

#
# This script is used to build Debian packages for Cligen.
#

# Make sure the script is started from the cligen directory
if [ ! -f scripts/version.sh ]; then
    echo "This script must be run from the cligen directory."
    exit 1
fi

VERSION=$(./scripts/version.sh)

if [ $? -ne 0 ]; then
    echo "Failed to determine the version of Cligen."
    exit 1
fi

# Create the build/ directory
if [ ! -d build ]; then
    mkdir build
fi

# Copy the debian/ directory to the build/ directory
if [ ! -d build/debian ]; then
    cp -r debian build/
fi

# Update the change log
echo -n "cligen (${VERSION}) " > build/debian/changelog

git --no-pager log --no-walk --encoding=utf-8 --expand-tabs=4 --pretty=format:"${VERSION} stable; urgency=medium%n%n * %w(,,2)%B%w()%n -- %an <%ae>  %aD%n" >> build/debian/changelog

if [ $? -ne 0 ]; then
    echo "Failed to update the change log."
    exit 1
fi

# Build Cligen and install it to the build/ directory
./configure && make clean && make && make install DESTDIR=build/

if [ $? -ne 0 ]; then
    echo "Failed to build Cligen."
    exit 1
fi

# Build the package
(cd build && dpkg-buildpackage -us -uc)

if [ $? -ne 0 ]; then
    echo "Failed to build the package."
    exit 1
fi
