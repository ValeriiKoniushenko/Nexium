pipeline {
    agent { label 'Windows' }

    environment {
        BUILD_TYPE = 'Debug'
    }

    stages {
        stage('Prepare') {
            steps {
                script {
                    isTriggeredByCron = currentBuild.getBuildCauses('hudson.triggers.TimerTrigger$TimerTriggerCause')

                    if (isTriggeredByCron) {
                        echo "Clean build preparation due to Cron task."
                        bat """
                            IF EXIST build rmdir /S /Q build
                        """
                    }
                }
            }
        }

        stage('Build') {
            steps {
                script {
                    def attempt = 0
                    def maxAttempts = 2
                    def success = false

                    while (!success && attempt < maxAttempts) {
                        attempt++

                        if (attempt == 2) {
                            echo "Previous build was FAILED. Let's try clear rebuild"
                            bat """
                            IF EXIST build rmdir /S /Q build
                            """
                        }
                        try {
                            bat """
                                cmake -S . -B build -G "Visual Studio 17 2022" -A x64

                                cmake --build build --config %BUILD_TYPE% -- /m:2
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
                    def BUILD_PATH = "build"
                    def ARCHIVE_NAME = "${BUILD_TYPE}-Win64.zip"

                    bat """
                        if exist ${ARCHIVE_NAME} del /Q ${ARCHIVE_NAME}
                        powershell Compress-Archive -Path ${BUILD_PATH}\\bin\\%BUILD_TYPE%\\* -DestinationPath ${ARCHIVE_NAME}
                    """
                    archiveArtifacts artifacts: "${ARCHIVE_NAME}", fingerprint: true
                }
            }
        }
    }
}
