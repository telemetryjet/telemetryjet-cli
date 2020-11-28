#!groovy

pipeline {
    agent none
    stages {
        stage('Build CLI') {
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
                            bat 'cmake .. -G \"MinGW Makefiles\"'
                        }
                        bat "cmake --build build --target TelemetryJetCli"
                    }
                }
            }
        }
    }
}