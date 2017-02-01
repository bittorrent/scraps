# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|

  config.vm.box = "macos1012"

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
    mkdir -p ~/workspace/scraps
    rsync -a --delete --exclude=.vagrant /vagrant/ ~/workspace/scraps/
    cd ~/workspace/scraps
    ./scripts/ci/install-xcode-dependencies
    ./scripts/ci/build-macos
  SHELL
end
