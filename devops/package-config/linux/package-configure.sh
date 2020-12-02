#!/bin/sh
set -e

# Run cmake configuration
mkdir -p build
cd build
cmake ..
cd ..

# Build the CLI
cmake --build build --target TelemetryJetCli
cd build

# Copy necessary files into manual installation bundle
rm -rf telemetryjet-cli
mkdir -p telemetryjet-cli
mkdir -p telemetryjet-cli/bin
cp jet telemetryjet-cli/bin
cp README.txt telemetryjet-cli
cp LICENSE.txt telemetryjet-cli

# Rename and zip up manual installation bundle
rm -f "telemetryjet-cli-linux_$2_$1.zip"
zip -r "telemetryjet-cli-linux_$2_$1.zip" telemetryjet-cli/

# Pull all files for the debian package into the folder and set proper file permissions
rm -rf telemetryjet-cli-package
mkdir -p telemetryjet-cli-package
mkdir -p telemetryjet-cli-package/usr/bin
rsync -avr ../devops/package-config/linux/deb/ telemetryjet-cli-package/

# Change architecture from armhf -> arm for the package description
ARCH=$1
if [ $2 -eq "armhf"]
then
  ARCH="arm"
fi

tee telemetryjet-cli-package/DEBIAN/control << END
Package: telemetryjet-cli
Version: $1
Section: utils
Priority: optional
Architecture: $ARCH
Maintainer: TelemetryJet Team <telemetryjet@gmail.com>
Homepage: https://www.telemetryjet.com/
Depends: 
Description: TelemetryJet CLI Information
 The TelemetryJet CLI is a utility for monitoring and controlling
 embedded hardware from your terminal. Pipe data from input devices
 into files, or stream directly to a TelemetryJet server.
 .
 See https://docs.dev.telemetryjet.com/cli/ for documentation and guides.

END
find ./telemetryjet-cli-package -type d | xargs chmod 755
objcopy jet ./telemetryjet-cli-package/usr/bin/jet --strip-debug --strip-unneeded
gzip --best -n ./telemetryjet-cli-package/usr/share/doc/telemetryjet-cli/changelog
gzip --best -n ./telemetryjet-cli-package/usr/share/doc/telemetryjet-cli/changelog.Debian
chmod 755 ./telemetryjet-cli-package/usr/bin/jet

# Build and test the package
if [ -z "$FAKEROOTKEY" ];
  then fakeroot;
fi;
dpkg-deb --build telemetryjet-cli-package
lintian telemetryjet-cli-package.deb

# Rename the debian package
mv telemetryjet-cli-package.deb "telemetryjet-cli-linux_$2_$1.deb"

cd ..
