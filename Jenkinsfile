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
                            bat 'copy "..\\devops\\windows-installer.iss" .'
                            bat 'copy jet.exe "TelemetryJet CLI"'
                            bat 'copy README.txt "TelemetryJet CLI"'
                            bat 'copy LICENSE.txt "TelemetryJet CLI"'
                            bat "if exist \"telemetryjet-cli-windows_${TAG_NAME}.zip\" del \"telemetryjet-cli-windows_${TAG_NAME}.zip\""
                            bat "7z a -tzip \"telemetryjet-cli-windows_${TAG_NAME}.zip\" \"TelemetryJet CLI\""
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
                            sh 'cp jet telemetryjet-cli'
                            sh 'cp README.txt telemetryjet-cli'
                            sh 'cp LICENSE.txt telemetryjet-cli'
                            sh "rm -f \"telemetryjet-cli-ubuntu_x86-64_${TAG_NAME}.zip\""
                            sh "zip -r \"telemetryjet-cli-ubuntu_x86-64_${TAG_NAME}.zip\" telemetryjet-cli/"
                            stash includes: "telemetryjet-cli-ubuntu_x86-64_${TAG_NAME}.zip", name: 'UBUNTU_BUILD_ARCHIVE'
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
                sh "yes | cp -rf \"telemetryjet-cli-windows_x86-64_${TAG_NAME}.zip\" /var/telemetryjet-downloads/builds/cli/windows/"
                sh "yes | cp -rf \"telemetryjet-cli-windows_x86-64_${TAG_NAME}.exe\" /var/telemetryjet-downloads/builds/cli/windows/"
                sh "yes | cp -rf \"telemetryjet-cli-ubuntu_x86-64_${TAG_NAME}.zip\" /var/telemetryjet-downloads/builds/cli/ubuntu/"
            }
        }
    }
}