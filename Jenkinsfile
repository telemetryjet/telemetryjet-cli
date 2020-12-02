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
                        bat 'if exist build rmdir build /q /s'
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
                stage('Build on MacOS') {
                    agent {
                        label "macos"
                    }
                    steps {
                        sh 'gcc --version'
                        sh 'cmake --version'
                        sh 'rm -rf build'
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
                stage('Build on Ubuntu') {
                    agent {
                        label "master"
                    }
                    steps {
                        sh "./devops/package-config/linux/package-configure.sh ${TAG_NAME} amd64"
                        dir('build') {
                            stash includes: "telemetryjet-cli-linux_amd64_${TAG_NAME}.zip", name: 'LINUX_AMD64_BUILD_ARCHIVE'
                            stash includes: "telemetryjet-cli-linux_amd64_${TAG_NAME}.deb", name: 'LINUX_AMD64_DEB_PACKAGE'
                        }
                    }
                }
                stage('Build on Raspberry Pi') {
                    agent {
                        label "rpi"
                    }
                    steps {
                        sh "./devops/package-config/linux/package-configure.sh ${TAG_NAME} armhf"
                        dir('build') {
                            stash includes: "telemetryjet-cli-linux_armhf_${TAG_NAME}.zip", name: 'LINUX_ARMHF_BUILD_ARCHIVE'
                            stash includes: "telemetryjet-cli-linux_armhf_${TAG_NAME}.deb", name: 'LINUX_ARMHF_DEB_PACKAGE'
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
                unstash 'LINUX_AMD64_BUILD_ARCHIVE'
                unstash 'LINUX_ARMHF_BUILD_ARCHIVE'
                unstash 'LINUX_AMD64_DEB_PACKAGE'
                unstash 'LINUX_ARMHF_DEB_PACKAGE'
                unstash 'MACOS_BUILD_ARCHIVE'
                sh 'sudo chown -R www-data:www-data *'
                sh "yes | cp -rf \"telemetryjet-cli-windows_x86-64_${TAG_NAME}.zip\" /var/telemetryjet-downloads/builds/cli/windows/"
                sh "yes | cp -rf \"telemetryjet-cli-windows_x86-64_${TAG_NAME}.exe\" /var/telemetryjet-downloads/builds/cli/windows/"
                sh "yes | cp -rf \"telemetryjet-cli-linux_amd64_${TAG_NAME}.zip\" /var/telemetryjet-downloads/builds/cli/linux/amd64/"
                sh "yes | cp -rf \"telemetryjet-cli-linux_amd64_${TAG_NAME}.deb\" /var/telemetryjet-downloads/builds/cli/linux/amd64/"
                sh "yes | cp -rf \"telemetryjet-cli-linux_armhf_${TAG_NAME}.zip\" /var/telemetryjet-downloads/builds/cli/linux/armhf/"
                sh "yes | cp -rf \"telemetryjet-cli-linux_armhf_${TAG_NAME}.deb\" /var/telemetryjet-downloads/builds/cli/linux/armhf/"
                sh "yes | cp -rf \"telemetryjet-cli-macos_x86-64_${TAG_NAME}.zip\" /var/telemetryjet-downloads/builds/cli/mac/"
                sh "./devops/package-config/linux/repository-configure.sh ${TAG_NAME} amd64"
                sh "./devops/package-config/linux/repository-configure.sh ${TAG_NAME} armhf"
            }
        }
    }
}