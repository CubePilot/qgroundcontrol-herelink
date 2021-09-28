#!/bin/sh

rm android/debug.keystore

keytool -genkey -v -keystore android/debug.keystore -storepass android -alias androiddebugkey -keypass android -keyalg RSA -keysize 2048 -validity 10000  
