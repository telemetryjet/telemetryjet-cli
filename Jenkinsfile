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
                            bat '''cmake .. -G \"CodeBlocks - MinGW Makefiles\"^
                            -D CMAKE_C_COMPILER=\"C:\\Program Files\\mingw-w64\\winlibs-x86_64-posix-seh-gcc-10.2.0-llvm-11.0.0-mingw-w64-8.0.0-r3\\mingw64\\bin\"^
                            -D CMAKE_CXX_COMPILER=\"C:\\Program Files\\mingw-w64\\winlibs-x86_64-posix-seh-gcc-10.2.0-llvm-11.0.0-mingw-w64-8.0.0-r3\\mingw64\\bin\"'''
                        }
                        bat "cmake --build build --target TelemetryJetCli"
                    }
                }
            }
        }
    }
}