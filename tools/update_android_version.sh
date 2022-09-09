#!/usr/bin/env bash

# Strip the 'v' from the beginning of the tag
VERSIONNAME=`git describe --always --tags | sed -e 's/^v//'`
echo "tag $VERSIONNAME"

# Change all occurences of '-' in tag to '.' and separate into parts
IFS=. read major minor patch dev sha <<<"${VERSIONNAME//-/.}"

major=4
minor=0
patch=8
dev=0
sha=$VERSIONNAME
echo "major:$major minor:$minor patch:$patch dev:$dev sha:$sha"

# Max Android version code is 2100000000. Version codes must increase with each release and the 
# version codes for multiple apks for the same release must be unique and not collide as well. 
# All of this makes it next to impossible to create a rational system of building a version code
# from a semantic version without imposing some strict restrictions.
if [ $major -gt 9 ]; then
    echo "Error: Major version larger than 1 digit: $major"
    exit 1
fi
if [ $minor -gt 9 ]; then
    echo "Error: Minor version larger than 1 digit: $minor"
    exit 1
fi
if [ $patch -gt 99 ]; then
    echo "Error: Patch version larger than 2 digits: $patch"
    exit 1
fi
if [ $dev -gt 999 ]; then
    echo "Error: Dev version larger than 3 digits: $dev"
    exit 1
fi

# Version code format: BBMIPPDDD (B=Bitness, I=Minor)
VERSIONCODE=$(($major*1000000))
VERSIONCODE=$(($(($minor*100000)) + $VERSIONCODE))
VERSIONCODE=$(($(($patch*1000)) + $VERSIONCODE))
VERSIONCODE=$(($(($dev)) + $VERSIONCODE))

# The 32 bit and 64 bit APKs each need there own version code.
if [ "$1" = "32" ]; then
    VERSIONCODE=33$VERSIONCODE
else
    VERSIONCODE=65$VERSIONCODE
fi

MANIFEST_FILE=android/AndroidManifest.xml

QGC_PKG_NAME="org.mavlink.qgroundcontrolvideomod"
sed -i -e 's/package *= *"[^"]*"/package="'$QGC_PKG_NAME'"/' $MANIFEST_FILE

# manifest package
if [ "$2" = "master" ]; then
	QGC_PKG_NAME="org.mavlink.qgroundcontrolbeta"
	sed -i -e 's/package *= *"[^"]*"/package="'$QGC_PKG_NAME'"/' $MANIFEST_FILE
	echo "Android package name: $QGC_PKG_NAME"
fi

# android:versionCode
if [ -n "$VERSIONCODE" ]; then
	sed -i -e "s/android:versionCode=\"[0-9][0-9]*\"/android:versionCode=\"$VERSIONCODE\"/" $MANIFEST_FILE
	echo "Android version: ${VERSIONCODE}"
else
	echo "Error versionCode empty"
	exit 0 # don't cause the build to fail
fi

# android:versionName
if [ -n "$VERSIONNAME" ]; then
	sed -i -e 's/versionName *= *"[^"]*"/versionName="'$VERSIONNAME'"/' $MANIFEST_FILE
	echo "Android name: ${VERSIONNAME}"
else
	echo "Error versionName empty"
	exit 0 # don't cause the build to fail
fi

