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
rm -f "telemetryjet-cli-ubuntu_x86-64_$1.zip"
zip -r "telemetryjet-cli-ubuntu_x86-64_$1.zip" telemetryjet-cli/

# Pull all files for the debian package into the folder and set proper file permissions
rm -rf telemetryjet-cli-package
mkdir -p telemetryjet-cli-package
mkdir -p telemetryjet-cli-package/usr/bin
rsync -avr ../devops/package-config/linux/deb/ telemetryjet-cli-package/
tee telemetryjet-cli-package/DEBIAN/control << END
Package: telemetryjet-cli
Version: $1
Section: utils
Priority: optional
Architecture: amd64
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
mv telemetryjet-cli-package.deb "telemetryjet-cli-ubuntu-x86_64_$1.deb"

cd ..

# Update repository
echo "===== UPDATING REPOSITORY ====== "
cp "build/telemetryjet-cli-ubuntu-x86_64_$1.deb" /var/telemetryjet-downloads/builds/cli/ubuntu/apt-repository/amd64
cd /var/telemetryjet-downloads/builds/cli/ubuntu/apt-repository/
dpkg-sig -k telemetryjet --sign repo "amd64/telemetryjet-cli-ubuntu-x86_64_$1.deb"
apt-ftparchive packages amd64 > Packages
gzip -k -f Packages
apt-ftparchive release . > Release
rm -fr Release.gpg; gpg --default-key telemetryjet -abs -o Release.gpg Release
rm -fr InRelease; gpg --default-key telemetryjet --clearsign -o InRelease Release
