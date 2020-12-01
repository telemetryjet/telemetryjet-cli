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
sh mkdir -p telemetryjet-cli
mkdir -p telemetryjet-cli/bin
cp jet telemetryjet-cli/bin
cp README.txt telemetryjet-cli
cp LICENSE.txt telemetryjet-cli

# Rename and zip up manual installation bundle
rm -f "telemetryjet-cli-ubuntu_x86-64_$1.zip"
zip -r "telemetryjet-cli-ubuntu_x86-64_$1.zip" telemetryjet-cli/

# Pull all files for the debian package into the folder and set proper file permissions
rm -rf telemetryjet-cli-package
mkdir -p telemetryjet-cli-package
mkdir -p telemetryjet-cli-package/usr/bin
rsync -avr ../devops/package-config/linux telemetryjet-cli-package/
find ./telemetryjet-cli-package -type d | xargs chmod 755
objcopy jet ./telemetryjet-cli-package/usr/bin/jet --strip-debug --strip-unneeded
gzip --best ./telemetryjet-cli-package/usr/share/doc/telemetryjet-cli/copyright
gzip --best ./telemetryjet-cli-package/usr/share/doc/telemetryjet-cli/changelog
gzip --best ./telemetryjet-cli-package/usr/share/doc/telemetryjet-cli/changelog.Debian
chmod 755 ./telemetryjet-cli-package/usr/bin/jet

# Build and test the package
fakeroot dpkg-deb --build telemetryjet-cli-package
lintian telemetryjet-cli-package.deb

# Rename the debian package
mv telemetryjet-cli-package.deb "telemetryjet-cli-ubuntu-x86_64_$1.deb"

cd ..