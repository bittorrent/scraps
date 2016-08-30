# Scraps [![Build Status](https://travis-ci.org/bittorrent/scraps.svg?branch=master)](https://travis-ci.org/bittorrent/scraps) [![Apache 2.0 License](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](https://raw.githubusercontent.com/bittorrent/scraps/master/LICENSE)

Useful scraps of C++ code. This isn't intended for widespread use, and we don't recommend that it be used directly. By open-sourcing it, we aim to...

* Promote transparency.
* Facilitate community involvement regarding issues.
* Publish code with broad application under a permissive enough license that it can be re-used elsewhere.
* Enable open sourcing of other internal projects that depend on Scraps.
* Further motivate ourselves to remain conscious of and maintain a higher standard of code quality.

Because this library isn't intended for widespread use as a whole, we will not be entertaining requests for new features that don't benefit our other projects.

# Status

| Branch | Status | Description |
| --- | --- | --- |
| master | [![Build Status](https://travis-ci.org/bittorrent/scraps.svg?branch=master)](https://travis-ci.org/bittorrent/scraps) | This is the most stable branch, with merges generally coinciding with releases. |
| develop | [![Build Status](https://travis-ci.org/bittorrent/scraps.svg?branch=develop)](https://travis-ci.org/bittorrent/scraps) | This is the most cutting edge branch, which we do day-to-day development in. |

## Building

### Prerequisites

You'll need to install these yourself if you don't have them already.

* The **pyyaml** and **jinja2** Python packages are used by Needy for our dependency configuration.
* **git** is used to retrieve several of our dependencies.
* **pkg-config** is used to integrate our dependencies.
* **makedepend** is required by OpenSSL.
* **clang** and **libc++** are required to compile.
* **libz** is required by Curl.
* **OpenGL** is required by Scraps.

These things must be installed before continuing.

### Configuring

The simplest way to configure is to invoke `./build-deps --configure`. This will download all of our third-party dependencies and configure the build to use them. It will also bootstrap `b2` if you don't already have it installed (which will be used in the compilation step).

You can override any of our dependencies by placing them in your `PKG_CONFIG_PATH` before configuring.

### Compiling

`b2` will compile the library for you. You can also use `b2 install --prefix=myinstalldir` to install it, along with all of the dependencies that you did not provide during the configure step.

## License

Scraps is provided under the Apache License, which can be found in the *LICENSE* file.
