pipeline {
    agent { label 'Linux' }

    stages {
        stage('Static Code Analysis') {
            steps {
                script {
                    sh '''
                        cppcheck --enable=all \
                            --suppress=missingIncludeSystem \
                            --suppress=unusedFunction \
                            --suppress=missingInclude \
                            --suppress=identicalInnerCondition \
                            --suppress=knownConditionTrueFalse \
                            --suppress=noExplicitConstructor \
                            --xml --xml-version=2 sources/ 2> cppcheck.xml
                    '''
                }
            }
        }
    }
    post {
        always {
            recordIssues(
                enabledForFailure: true,
                tools: [
                    cppCheck(pattern: 'cppcheck.xml')
                ]
            )
        }
    }
}
