stage('Build') {
    parallel linux: {
        node('ubuntu') {
            checkout scm
            sh 'git submodule update --init --recursive'
            sh 'docker-compose run --rm linux-env bash -c "./scripts/ci/install-linux-dependencies && ./scripts/ci/build-linux"'
        }
    }, android: {
        node('ubuntu') {
            checkout scm
            sh 'git submodule update --init --recursive'
            sh 'docker-compose run --rm android-env bash -c "./scripts/ci/install-android-dependencies && ./scripts/ci/build-android"'
        }
    }
}
