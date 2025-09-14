pipeline {
    agent none

    environment {
        REPORT_DIR = 'build_reports'
    }

    triggers {
        cron('H 3 * * *')
    }

    stages {
        stage('Static Code Analysis') {
            agent { label 'Linux' }

            steps {
                script {
                    sh '''
                        mkdir -p ${REPORT_DIR}

                        cppcheck --enable=all \
                            --suppress=missingIncludeSystem \
                            --suppress=unusedFunction \
                            --suppress=missingInclude \
                            --suppress=unknownMacro \
                            --suppress=identicalInnerCondition \
                            --suppress=knownConditionTrueFalse \
                            --suppress=syntaxError \
                            --xml --xml-version=2 sources/ 2> ${REPORT_DIR}/cppcheck.xml
                    '''
                }
            }
        }

        stage('Linux builds') {
            matrix {
                axes {
                    axis {
                        name 'COMPILER_PAIR'
                        values 'gcc:g++', 'clang:clang++'
                    }

                    axis {
                        name 'BUILD_TYPE'
                        values 'Debug', 'Release'
                    }
                }

                agent { label 'Linux' }

                stages {
                    stage('Prepare') {
                        steps {
                            script {
                                def causes = currentBuild.getBuildCauses()
                                echo "Causes: ${causes}"

                                def isTriggeredByCron = causes.any { it._class == 'hudson.triggers.TimerTrigger$TimerTriggerCause' }
                                if (isTriggeredByCron) {
                                    echo "Clean build preparation due to Cron task."
                                    sh """
                                        rm -rf build
                                    """
                                }
                            }
                        }
                    }

                    stage('Configure & Build') {
                        steps {
                            script {
                                def (C_COMPILER, CPP_COMPILER) = COMPILER_PAIR.split(':')

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

                                            cmake --build ${buildDir} -- -j2
                                        """
                                        success = true
                                    } catch(err) {
                                        echo "Build failed on attempt #${attempt}"
                                        if (attempt == maxAttempts) {
                                            error "Build failed after ${maxAttempts} attempts"
                                        }
                                    }
                                }

                                addEmbeddableBadgeConfiguration(
                                    id: "linuxBuild_${C_COMPILER}_${BUILD_TYPE}",
                                    subject: "Linux | ${C_COMPILER} | ${BUILD_TYPE}",
                                    status: (success ? "success" : "failed"),
                                    color: (success ? "green" : "red")
                                )
                            }
                        }
                    }
                }
            }
        }

        stage('Windows builds') {
            matrix {
                axes {
                    axis {
                        name 'BUILD_TYPE'
                        values 'Debug', 'Release'
                    }
                }

                agent { label 'Windows' }

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

                    stage('Configure & Build') {
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

                                addEmbeddableBadgeConfiguration(
                                    id: "windowsBuild_${BUILD_TYPE}",
                                    subject: "Win11 | MSVC | ${BUILD_TYPE}",
                                    status: (success ? "success" : "failed"),
                                    color: (success ? "green" : "red")
                                )
                            }
                        }
                    }
                }
            }
        }
    }

    post {
        always {
            node('Linux') {
                recordIssues(
                    enabledForFailure: true,
                    tools: [
                        cppCheck(pattern: 'build_reports/cppcheck.xml')
                    ]
                )
            }
        }
    }
}
