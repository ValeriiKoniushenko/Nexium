pipeline {
    agent { label 'Linux' }

    environment {
        C_COMPILER = 'clang'
        CPP_COMPILER = 'clang++'
        BUILD_TYPE = 'Release'
    }

    stages {
        stage('Prepare') {
            steps {
                script {
                    isTriggeredByCron = currentBuild.getBuildCauses('hudson.triggers.TimerTrigger$TimerTriggerCause')
                    if (isTriggeredByCron) {
                        echo "Clean build preparation due to Cron task."
                        sh """
                            rm -rf build
                        """
                    }
                }
            }
        }

        stage('Build') {
            steps {
                script {
                    def buildDir = "build/${C_COMPILER}/${BUILD_TYPE}"
                    def attempt = 0
                    def maxAttempts = 2
                    def success = false

                    while (!success && attempt < maxAttempts) {
                        attempt++

                        if (attempt == 2) {
                            echo "Previous build was FAILED. Let's try clear rebuild"
                            sh """
                            rm -rf build
                            """
                        }
                        try {
                            sh """
                                cmake -S . -B ${buildDir} \
                                      -DCMAKE_C_COMPILER=${C_COMPILER}          \
                                      -DCMAKE_CXX_COMPILER=${CPP_COMPILER}      \
                                      -DCMAKE_BUILD_TYPE=${BUILD_TYPE}          \
                                      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

                                cmake --build ${buildDir} -- -j4
                            """
                            success = true
                        } catch(err) {
                            echo "Build failed on attempt #${attempt}"
                            if (attempt == maxAttempts) {
                                error "Build failed after ${maxAttempts} attempts"
                            }
                        }
                    }
                }
            }
        }

        stage('Package Artifacts') {
            steps {
                script {
                    def BUILD_PATH = "build/${C_COMPILER}/${BUILD_TYPE}"
                    def ARCHIVE_NAME = "${C_COMPILER}-${BUILD_TYPE}.tar.gz"

                    sh """
                        rm -f ${ARCHIVE_NAME}
                        tar czf ${ARCHIVE_NAME} ${BUILD_PATH}/bin/*
                    """
                    archiveArtifacts artifacts: "${ARCHIVE_NAME}", fingerprint: true
                }
            }
        }
    }
}
