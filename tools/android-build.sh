#!/bin/bash
# Script to cross compile utox for android from linux (and soon windows (maybe not...))
#


usage() {
	echo "Cross compile script to build uTox for android from a ~unix environment."
	echo
	echo "Usage: "
	echo "cd [utox_root_directory/]"
	echo "$0"
	echo
	echo "Yup, that's it..."
	return 0
}

for arg in "$@"; do
	case "$arg" in
		*)
			usage
			exit 1
			;;
	esac
done

export ALIAS=utox

export ANDROID_NDK_HOME=/opt/android-ndk
mkdir toolchain
cd toolchain
export TOOLCHAIN=$(pwd)
cd ..
"$ANDROID_NDK_HOME/build/tools/make-standalone-toolchain.sh" \
 		--ndk-dir="$ANDROID_NDK_HOME" \
 		--toolchain=arm-linux-androideabi-clang3.5 \
 		--install-dir=$TOOLCHAIN \
 		--platform=android-9

export PATH="$TOOLCHAIN/bin:$PATH"
SDK_PATH=/opt/android-sdk
# cd utox

cd src/

mkdir ./tmp
mkdir ./tmp/java
mkdir ./tmp/libs
mkdir ./tmp/libs/armeabi

ls -la ../lib/openal-arm/lib/
OPENAL_BUILD='-I../lib/openal-arm/include ../lib/openal-arm/lib/libopenal.a -lOpenSLES'
NATIVE_AUDIO_BUILD='-DNATIVE_ANDROID_AUDIO -lOpenSLES'

arm-linux-androideabi-gcc -Wl,--error-unresolved-symbols \
		-Wall -Wextra -s \
		-I../lib/freetype-arm/include/freetype2/ -I../lib/toxcore-arm/include/ \
		./*.c ./png/png.c -llog -landroid -lEGL -lGLESv2 $OPENAL_BUILD \
		../lib/toxcore-arm/lib/libtoxcore.a ../lib/toxcore-arm/lib/libtoxdns.a ../lib/toxcore-arm/lib/libtoxav.a ../lib/toxcore-arm/lib/libsodium.a \
		../lib/toxcore-arm/lib/libopus.a ../lib/toxcore-arm/lib/libvpx.a ../lib/freetype-arm/lib/libfreetype.a \
		-lm -lz -ldl -shared -o ./tmp/libs/armeabi/libn.so

$SDK_PATH/build-tools/21.1.2/aapt package -f -M ./android/AndroidManifest.xml -S ./android/res \
		-I $SDK_PATH/platforms/android-21/android.jar -F ./tmp/tmp1.apk -J ./tmp/java

javac -d ./tmp/java ./tmp/java/R.java

$SDK_PATH/build-tools/21.1.2/dx --dex --output=./tmp/classes.dex ./tmp/java

java -classpath $SDK_PATH/tools/lib/sdklib.jar com.android.sdklib.build.ApkBuilderMain ./tmp/tmp2.apk \
    	-u -z ./tmp/tmp1.apk -f ./tmp/classes.dex -nf ./tmp/libs

jarsigner -sigalg SHA1withRSA -digestalg SHA1 -keystore ./debug.keystore -storepass $PASSWORD ./tmp/tmp2.apk $ALIAS

mv tmp/tmp2.apk ./utox.apk

rm -r tmp toolchain

