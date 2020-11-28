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
                        bat 'if not exist build mkdir build'
                        dir('build') {
                            bat 'cmake .. -G \"CodeBlocks - MinGW Makefiles\"'
                        }
                        bat "cmake --build build --target TelemetryJetCli"
                    }
                }
            }
        }
    }
}