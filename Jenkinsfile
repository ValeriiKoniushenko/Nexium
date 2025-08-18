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
                                    }
                                    try {
                                        sh """
                                            # rm -rf build

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

                    /*stage('Run') {
                        steps {
                            script {
                                def (C_COMPILER, CPP_COMPILER) = COMPILER_PAIR.split(':')
                                def success = false

                                try {
                                    sh """
                                        cd build/${C_COMPILER}/${BUILD_TYPE}/bin
                                        ./TestEngine --gtest_output=xml:gtest_result.xml
                                        cd ../../../../
                                    """
                                    success = true
                                } catch(err){
                                }

                                addEmbeddableBadgeConfiguration(
                                    id: "linuxTests_${C_COMPILER}_${BUILD_TYPE}",
                                    subject: "Linux | ${C_COMPILER} | ${BUILD_TYPE}",
                                    status: (success ? "passed" : "failed"),
                                    color: (success ? "green" : "red")
                                )
                            }
                        }
                    }*/

                    /*stage('Gen Test Results') {
                        when {
                           expression { BUILD_TYPE == 'Debug' }
                        }

                        steps {
                            script {
                                def (C_COMPILER, CPP_COMPILER) = COMPILER_PAIR.split(':')

                                junit "build/${C_COMPILER}/${BUILD_TYPE}/bin/gtest_result.xml"
                            }
                        }
                    }*/

                    /*stage('Test Coverage') {
                        when {
                           expression { BUILD_TYPE == 'Debug' && COMPILER_PAIR == "clang:clang++" }
                        }

                        steps {
                            script {
                                def BIN_PATH = "build/clang/Debug/bin"

                                sh """
                                    llvm-profdata merge -output=${BIN_PATH}/default.profdata ${BIN_PATH}/default.profraw

                                    llvm-cov show ./${BIN_PATH}/TestEngine \
                                        -instr-profile=${BIN_PATH}/default.profdata \
                                        -format=html \
                                        -output-dir=coverage_report \
                                        --ignore-filename-regex="(build/.*)|(tests/.*)"
                                """

                                publishHTML([reportDir: 'coverage_report', reportFiles: 'index.html', reportName: 'LLVM Coverage'])

                                def report = readFile("coverage_report/index.html")

                                def coverage = 0.0
                                def m = (report =~ /Totals\s*\<\/pre\>\<\/td\>\<td\s+class='[\w-]+'\>\<pre\>\s*([\d\.]+)%/)
                                if (m.find()) {
                                    coverage = m.group(1).toDouble()
                                }
                                m = null

                                def color = coverage > 90 ? "green" :
                                            coverage > 70 ? "yellow" : "red"

                                def badgeJson = """{
                                    "schemaVersion": 1,
                                    "label": "coverage",
                                    "message": "${coverage}%",
                                    "color": "${color}"
                                }"""
                                writeFile(file: "coverage-badge.json", text: badgeJson)
                                archiveArtifacts artifacts: "coverage-badge.json", fingerprint: true

                            }
                        }
                    }*/

                    /*stage('Package Artifacts') {
                        when {
                           expression { BUILD_TYPE == 'Release' }
                        }

                        steps {
                            script {
                                def (C_COMPILER, CPP_COMPILER) = COMPILER_PAIR.split(':')
                                def BUILD_PATH = "build/${C_COMPILER}/${BUILD_TYPE}"
                                def ARCHIVE_NAME = "${env.JOB_NAME}-${C_COMPILER}.tar.gz"

                                sh """
                                    rm -f ${ARCHIVE_NAME}
                                    tar czf ${ARCHIVE_NAME} -C ${BUILD_PATH}/bin .
                                """
                                archiveArtifacts artifacts: "${ARCHIVE_NAME}", fingerprint: true
                            }
                        }
                    }*/
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

                    /*stage('Run') {
                        steps {
                            script {
                                def success = false
                                try {
                                    bat """
                                        build\\bin\\%BUILD_TYPE%\\TestEngine.exe --gtest_output=xml:build\\bin\\%BUILD_TYPE%\\gtest_result.xml
                                    """
                                    success = true
                                } catch(err) {
                                }

                                addEmbeddableBadgeConfiguration(
                                    id: "windowsTests_${BUILD_TYPE}",
                                    subject: "Win11 | ${BUILD_TYPE}",
                                    status: (success ? "passed" : "failed"),
                                    color: (success ? "green" : "red")
                                )
                            }
                        }
                    }*/

                    /*stage('Package Artifacts') {
                        when {
                            expression { BUILD_TYPE == 'Release' }
                        }
                        steps {
                            script {
                                def BUILD_PATH = "build"
                                def ARCHIVE_NAME = "${env.JOB_NAME}-Win64.zip"

                                bat """
                                    if exist ${ARCHIVE_NAME} del /Q ${ARCHIVE_NAME}
                                    powershell Compress-Archive -Path ${BUILD_PATH}\\bin\\%BUILD_TYPE%\\* -DestinationPath ${ARCHIVE_NAME}
                                """
                                archiveArtifacts artifacts: "${ARCHIVE_NAME}", fingerprint: true
                            }
                        }
                    }*/
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
