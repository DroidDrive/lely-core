![logo](doc/logo.png)

Utilities library
=================

Introduction
------------

The Lely utilities library ([liblely-util]) provides a variety of functions and
data structures used by, and shared between, other Lely libraries.

Download
--------

liblely-util is hosted on [GitLab]. You can clone the repository with

    $ git clone https://gitlab.com/lely_industries/util.git

or download the latest release
([v1.4.0](https://gitlab.com/lely_industries/util/tags/v1.4.0)).

Build and install
-----------------

liblely-util uses the GNU Build System (`configure`, `make`, `make install`) and
has native support for POSIX platforms (Debian-based Linux distributions in
particular) and Windows (Vista and higher) through [Cygwin] or [MinGW-w64].
[GCC ARM Embedded] and Microsoft Visual Studio (2013 and higher) are also
supported, but project files will have to be created by hand.

The release archive includes the build system, but if you want to build a
checkout of the repository, you need to install the autotools (autoconf,
automake and libtool). The build system can then be generated by running

    $ autoreconf -i

in the root directory of the repository.

If you want to generate HTML documentation from the source code, you need to
install [doxygen] and [Graphviz].

liblely-util requires the Lely C11 and POSIX compatibility library
([liblely-libc]) to be installed.

### Debian packages

For Debian-based Linux distributions, the preferred installation method is to
generate and install Debian packages. Install the Debian package build system
with

    $ sudo apt-get install cdbs devscripts

The packages can now be built by running

    $ debuild -uc -us -b

from the root directory of the project. Once the packages are generated, you can
clean up the project directory with

    $ debuild clean

debuild creates the packages in the parent directory of the project. Install
them with

    $ cd ..
    $ sudo dpkg -i liblely-util*.deb

### configure, make, make install

It is also possible to build and install the library by hand. First, configure
the build system by running

    $ ./configure

from the root directory of the project.

The `configure` script supports many options. The full list can be shown with

    $ ./configure --help

Once the build system is configured, the library can be built with

    $ make

If the Lely TAP library ([liblely-tap]) is installed, the test suite can be run
with

    $ make check

Run

    $ make html

to generate the HTML documentation from the source code. The binaries, headers
and documentation can be installed by running

    # make install

as root.

### Options

liblely-util uses the multithreading functions of liblely-libc. If
multithreading support was disabled or unavailable when liblely-libc was built,
it should also be disabled when building liblely-util. This can be achieved by
providing the `--disable-threads` option to `configure` or by defining the
`LELY_NO_THREADS` preprocessor macro.

On POSIX and Windows platforms, liblely-util provides functions to run a process
in the background as a daemon/service. This functionality can be disabled with
the `--disable-daemon` option to `configure` or by defining the `LELY_NO_DAEMON`
preprocessor macro.

Although the Lely libraries are written in C, C++ interfaces are provided for a
subset of the functionality. These interfaces require support from liblely-util.
This support can be disabled with the `--disable-cxx` option to `configure` or
by defining the `LELY_NO_CXX` preprocessor macro.

The memory pool allocator allocates its memory in pages. By default these pages
are 64 KiB large and aligned on a 4 KiB boundary. For embedded systems this may
be far too large. The `LELY_PAGE_SIZE` and `LELY_PAGE_ALIGNMENT` preprocessor
macros can be used to set the size and alignment of memory pages (in bytes).

Usage
-----

See [doc/overview.md](@ref md_doc_overview) for an overview of the provided
functionality. Doxygen documentation for the latest development version can be
found at http://lely_industries.gitlab.io/util/doxygen/.

Licensing
---------

Copyright 2016 [Lely Industries N.V.]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

[Cygwin]: https://www.cygwin.com/
[doxygen]: http://www.doxygen.org/
[GCC ARM Embedded]: https://launchpad.net/gcc-arm-embedded
[GitLab]: https://gitlab.com/lely_industries/util
[Graphviz]: http://www.graphviz.org/
[liblely-libc]: https://gitlab.com/lely_industries/libc
[liblely-tap]: https://gitlab.com/lely_industries/tap
[liblely-util]: https://gitlab.com/lely_industries/util
[Lely Industries N.V.]: http://www.lely.com
[MinGW-w64]: http://mingw-w64.org/

