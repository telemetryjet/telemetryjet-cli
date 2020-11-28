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
                        bat 'iscc'
                        bat 'if not exist build mkdir build'
                        dir('build') {
                            bat 'cmake .. -G \"MinGW Makefiles\"'
                        }
                        bat 'cmake --build build --target TelemetryJetCli'
                        dir('build') {
                            bat 'if exist "TelemetryJet CLI" rd /s /q "TelemetryJet CLI"'
                            bat 'mkdir "TelemetryJet CLI"'
                            bat 'copy jet.exe "TelemetryJet CLI"'
                            bat 'copy README.txt "TelemetryJet CLI"'
                            bat 'copy LICENSE.txt "TelemetryJet CLI"'
                            bat "if exist \"telemetryjet-cli-windows_${TAG_NAME}.zip\" del \"telemetryjet-cli-windows_${TAG_NAME}.zip\""
                            bat "7z a -tzip telemetryjet-cli-windows_${TAG_NAME}.zip \"TelemetryJet CLI\""
                            stash includes "telemetryjet-cli-windows_${TAG_NAME}.zip", name: 'WINDOWS_BUILD_ARCHIVE'
                        }
                    }
                }
            }
        }
        stage ('Upload Windows Build Artifact') {
            when {
                buildingTag()
            }
            agent {
                label "master"
            }
            steps {
                unstash 'WINDOWS_BUILD_ARCHIVE'
                sh "yes | cp -rf \"telemetryjet-cli-windows_${TAG_NAME}.zip\" /var/telemetryjet-downloads/builds/cli/windows/"
            }
        }
    }
}