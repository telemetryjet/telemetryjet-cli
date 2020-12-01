#!groovy

pipeline {
    agent none
    stages {
        stage('Build CLI') {
            when {
                buildingTag()
            }
            parallel {
                stage('Build on Windows') {
                    agent {
                        label "windows"
                    }
                    steps {
                        bat 'gcc --version'
                        bat 'cmake --version'
                        bat 'if not exist build mkdir build'
                        dir('build') {
                            bat 'cmake .. -G "MinGW Makefiles"'
                        }
                        bat 'cmake --build build --target TelemetryJetCli'
                        dir('build') {
                            bat 'if exist "TelemetryJet CLI" rd /s /q "TelemetryJet CLI"'
                            bat 'mkdir "TelemetryJet CLI"'
                            bat 'mkdir "TelemetryJet CLI\\bin"'
                            bat 'copy "..\\devops\\package-config\\windows\\environment.iss" .'
                            bat 'copy "..\\devops\\package-config\\windows\\windows-installer.iss" .'
                            bat 'copy jet.exe "TelemetryJet CLI\\bin"'
                            bat 'copy README.txt "TelemetryJet CLI"'
                            bat 'copy LICENSE.txt "TelemetryJet CLI"'
                            bat "if exist \"telemetryjet-cli-windows_${TAG_NAME}.zip\" del \"telemetryjet-cli-windows_${TAG_NAME}.zip\""
                            bat "7z a -tzip \"telemetryjet-cli-windows_x86-64_${TAG_NAME}.zip\" \"TelemetryJet CLI\""
                            bat "iscc /dApplicationVersion=\"${TAG_NAME}\" \"windows-installer.iss\""
                            stash includes: "telemetryjet-cli-windows_x86-64_${TAG_NAME}.zip", name: 'WINDOWS_BUILD_ARCHIVE'
                            stash includes: "telemetryjet-cli-windows_x86-64_${TAG_NAME}.exe", name: 'WINDOWS_BUILD_INSTALLER'
                        }
                    }
                }
                stage('Build on Ubuntu') {
                    agent {
                        label "master"
                    }
                    steps {
                        sh 'gcc --version'
                        sh 'cmake --version'
                        sh 'mkdir -p build'
                        dir('build') {
                            sh 'cmake ..'
                        }
                        sh 'cmake --build build --target TelemetryJetCli'
                        dir('build') {
                            sh 'rm -rf telemetryjet-cli'
                            sh 'mkdir -p telemetryjet-cli'
                            sh 'mkdir -p telemetryjet-cli/bin'
                            sh 'cp jet telemetryjet-cli/bin'
                            sh 'cp README.txt telemetryjet-cli'
                            sh 'cp LICENSE.txt telemetryjet-cli'
                            sh "rm -f \"telemetryjet-cli-ubuntu_x86-64_${TAG_NAME}.zip\""
                            sh "zip -r \"telemetryjet-cli-ubuntu_x86-64_${TAG_NAME}.zip\" telemetryjet-cli/"
                            stash includes: "telemetryjet-cli-ubuntu_x86-64_${TAG_NAME}.zip", name: 'UBUNTU_BUILD_ARCHIVE'

                            sh 'rm -rf telemetryjet-cli-package'
                            sh 'mkdir -p telemetryjet-cli-package'
                            sh 'rsync ../devops/package-config/linux telemetryjet-cli-package/'
                            sh 'find ./telemetryjet-cli-package -type d | xargs chmod 755'
                            sh 'objcopy jet ./telemetryjet-cli-package/usr/bin/jet --strip-debug --strip-unneeded'
                            sh 'chmod ./telemetryjet-cli-package/usr/bin/jet 755'
                            sh "fakeroot dpkg-deb --build telemetryjet-cli-package"
                            sh "mv telemetryjet-cli-package.deb \"telemetryjet-cli-ubuntu-x86_64_${TAG_NAME}.deb\""
                        }
                    }
                }
                stage('Build on MacOS') {
                    agent {
                        label "macos"
                    }
                    steps {
                        sh 'gcc --version'
                        sh 'cmake --version'
                        sh 'mkdir -p build'
                        dir('build') {
                            sh 'cmake ..'
                        }
                        sh 'cmake --build build --target TelemetryJetCli'
                        dir('build') {
                            sh 'rm -rf telemetryjet-cli'
                            sh 'mkdir -p telemetryjet-cli'
                            sh 'mkdir -p telemetryjet-cli/bin'
                            sh 'cp jet telemetryjet-cli/bin'
                            sh 'cp README.txt telemetryjet-cli'
                            sh 'cp LICENSE.txt telemetryjet-cli'
                            sh "rm -f \"telemetryjet-cli-macos_x86-64_${TAG_NAME}.zip\""
                            sh "zip -r \"telemetryjet-cli-macos_x86-64_${TAG_NAME}.zip\" telemetryjet-cli/"
                            stash includes: "telemetryjet-cli-macos_x86-64_${TAG_NAME}.zip", name: 'MACOS_BUILD_ARCHIVE'
                        }
                    }
                }
                stage('Build on Raspberry Pi') {
                    agent {
                        label "rpi"
                    }
                    steps {
                        sh 'gcc --version'
                        sh 'cmake --version'
                        sh 'mkdir -p build'
                        dir('build') {
                            sh 'cmake ..'
                        }
                        sh 'cmake --build build --target TelemetryJetCli'
                        dir('build') {
                            sh 'rm -rf telemetryjet-cli'
                            sh 'mkdir -p telemetryjet-cli'
                            sh 'mkdir -p telemetryjet-cli/bin'
                            sh 'cp jet telemetryjet-cli/bin'
                            sh 'cp README.txt telemetryjet-cli'
                            sh 'cp LICENSE.txt telemetryjet-cli'
                            sh "rm -f \"telemetryjet-cli-rpi_arm_${TAG_NAME}.zip\""
                            sh "zip -r \"telemetryjet-cli-rpi_arm_${TAG_NAME}.zip\" telemetryjet-cli/"
                            stash includes: "telemetryjet-cli-rpi_arm_${TAG_NAME}.zip", name: 'RPI_BUILD_ARCHIVE'
                        }
                    }
                }
            }
        }
        stage('Upload Build Artifacts') {
            when {
                buildingTag()
            }
            agent {
                label "master"
            }
            steps {
                unstash 'WINDOWS_BUILD_ARCHIVE'
                unstash 'WINDOWS_BUILD_INSTALLER'
                unstash 'UBUNTU_BUILD_ARCHIVE'
                unstash 'RPI_BUILD_ARCHIVE'
                unstash 'MACOS_BUILD_ARCHIVE'
                sh "yes | cp -rf \"telemetryjet-cli-windows_x86-64_${TAG_NAME}.zip\" /var/telemetryjet-downloads/builds/cli/windows/"
                sh "yes | cp -rf \"telemetryjet-cli-windows_x86-64_${TAG_NAME}.exe\" /var/telemetryjet-downloads/builds/cli/windows/"
                sh "yes | cp -rf \"telemetryjet-cli-ubuntu_x86-64_${TAG_NAME}.zip\" /var/telemetryjet-downloads/builds/cli/ubuntu/"
                sh "yes | cp -rf \"telemetryjet-cli-rpi_arm_${TAG_NAME}.zip\" /var/telemetryjet-downloads/builds/cli/raspberrypi/"
                sh "yes | cp -rf \"telemetryjet-cli-macos_x86-64_${TAG_NAME}.zip\" /var/telemetryjet-downloads/builds/cli/mac/"
            }
        }
    }
}