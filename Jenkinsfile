stage('Build') {
    parallel linux: {
        node('ubuntu') {
            checkout scm
            tool name: 'Docker', type: 'org.jenkinsci.plugins.docker.commons.tools.DockerTool'
            sh 'docker-compose run --rm linux-env bash -c "./scripts/ci/install-linux-dependencies && ./scripts/ci/build-linux"'
        }
    }, android: {
        node('ubuntu') {
            checkout scm
            tool name: 'Docker', type: 'org.jenkinsci.plugins.docker.commons.tools.DockerTool'
            sh 'docker-compose run --rm android-env bash -c "./scripts/ci/install-android-dependencies && ./scripts/ci/build-android"'
        }
    }
}
