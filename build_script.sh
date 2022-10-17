#!/bin/bash

export GSTREAMER_NAME='arm64'
export BITNESS='64'

export SOURCE_FOLDER=$PWD
export ANDROID_HOME=/usr/home/bendraper/Android/Sdk
export PATH=$ANDROID_HOME/tools/bin:$PATH

export BITNESS='64'
export GSTREAMER_NAME='arm64'

export SHADOW_BUILD_DIR=$SOURCE_FOLDER/build/qgc/
export CODESIGN=nocodesign
export SPEC=android-clang
export CONFIG=installer

git submodule update --init --recursive

export ANDROID_NDK_ROOT=`pwd`/android-ndk-r20 
export ANDROID_SDK_ROOT=/home/bendraper/Android/Sdk
export PATH=`pwd`/android-ndk-r20:$PATH

export PATH=/tmp/Qt5.12-android_arm64_v8a/5.12.6/android_arm64_v8a/bin:$PATH

# Compute version number
./tools/update_android_version.sh ${BITNESS} notmaster

# Build
mkdir -p ${SHADOW_BUILD_DIR} 
cd ${SHADOW_BUILD_DIR}

export STABLE_OR_DAILY=StableBuild

qmake -r $SOURCE_FOLDER/qgroundcontrol.pro CONFIG+=${CONFIG} CONFIG+=${STABLE_OR_DAILY} -spec ${SPEC}

make -j16 VERBOSE=1 install INSTALL_ROOT=./release/android-build/
