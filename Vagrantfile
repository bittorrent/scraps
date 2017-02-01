# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|

  config.vm.box = "macos1012"

  # To our public users, sorry! You won't have access to this box due to Apple's
  # licensing restrictions. See https://github.com/bittorrent/live-build-environments
  # to see how to build your own. When you do, just remove the following line when
  # you have a box named macos1012 registered on your local machine.
  config.vm.box_url = "s3://live-dev-deps/atlas/box/vmware/macos1012-0.1.0.box"

  config.vm.provider "vmware_fusion" do |vb|
    vb.vmx["numvcpus"] = "8"
    vb.gui = false
    vb.memory = "2048"
  end

  # dependencies
  config.vm.provision "shell", privileged: false, inline: <<-SHELL
    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  SHELL

  # build
  config.vm.provision "shell", privileged: false, env: { "TARGET" => "macos" }, inline: <<-SHELL
    #!/bin/bash -e

    # TODO: the synchronization part of this block is really just a workaround
    # for problems compiling source directly from the /vagrant directory. If
    # possible, that should probably be done at some point in the future.
    mkdir -p ~/workspace/scraps
    echo "rsyncing source to local directory"
    time rsync -a --delete --exclude=/.vagrant /vagrant/ ~/workspace/scraps/
    cd ~/workspace/scraps
    ./scripts/ci/install-xcode-dependencies
    ./scripts/ci/build-macos
    echo "rsyncing results to host"
    mkdir -p /vagrant/dist
    time rsync -a --delete -v ~/workspace/scraps/dist/ /vagrant/dist/
  SHELL
end
