# Scraps

Useful scraps of C++ code. This contains whatever functionality we find to be generally useful across our codebases.

## Building

### Prerequisites

You'll need to install these yourself if they aren't already.

* The **pyyaml** and **jinja2** Python packages are used by Needy for our dependency configuration.
* **git** is used to retrieve several of our dependencies.
* **pkg-config** is used to integrate our dependencies.
* **makedepend** is required by OpenSSL.

These things must be installed before continuing.

### Configuring

The simplest way to configure is to invoke `./build-deps --configure`. This will download all of our third-party dependencies and configure the build to use them. It will also bootstrap `b2` if you don't already have it installed (which will be used in the compilation step).

You can override any of our dependencies by placing them in your `PKG_CONFIG_PATH` before configuring.

### Compiling

`b2` will compile the library for you. You can also use `b2 install --prefix=myinstalldir` to install it, along with all of the dependencies that you did not provide during the configure step.
