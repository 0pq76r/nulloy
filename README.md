# Nulloy Music Player [![Build Status](https://travis-ci.org/sergey-vlasov/nulloy.png?branch=devel)](https://travis-ci.org/sergey-vlasov/nulloy)

**Testing PPA:** [ppa:sergey-vlasov/nulloy-testing](http://launchpad.net/~sergey-vlasov/+archive/nulloy-testing)



## Windows Build Instructions

### Downloads
* Qt 4.x MinGW libraries for Windows http://qt-project.org/downloads
* MinGW http://sourceforge.net/projects/mingw-w64/
* GStreamer Build & SDK http://code.google.com/p/ossbuild/
* CMake http://www.cmake.org/
* TagLib Source http://taglib.github.io/
* Zlib precompiled dlls http://zlib.net/
* 7zip http://www.7-zip.org/

### Environment setup

Extract and/or install the downloads. Create ```vars.bat``` file with:

<pre>
set QTDIR=C:\qt-libs-dir
set TAGLIB_DIR=C:\taglib-source-dir
set ZLIB_DIR=C:\zlib-dlls-dir
set PATH=%QTDIR%\bin;C:\mingw-dir\bin;C:\Program Files\7-Zip;%ZLIB_DIR%;%PATH%
</pre>

Create a shortcut from ```vars.bat``` and set target as ```%COMSPEC% /k "C:\vars.bat"```. Open the shortcut.

### Building TagLib

<pre>
cd %TAGLIB_DIR%
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%TAGLIB_DIR% -DCMAKE_RELEASE_TYPE=Release -DENABLE_STATIC=ON -DENABLE_STATIC_RUNTIME=ON .
mingw32-make
mingw32-make install
</pre>

### Build & Run Nulloy

<pre>
cd C:\nulloy.git
configure.bat --taglib --no-gstreamer-tagreader --force-version 1-testing
mingw32-make
Nulloy.exe
</pre>



## OSX Build Instructions

### Downloads
* Xcode
* MacPorts Installer http://www.macports.org/
* Qt 4.x libraries for Mac http://qt-project.org/downloads

### Environment setup

Extract Qt libraries. Install Xcode and then install Xcode Command Line Tools. Install MacPorts and open a terminal.

<pre>
. ~/.profile
export QTDIR=$HOME/qt-libs-dir
export PATH=$QTDIR/bin:$PATH
sudo port install pkgconfig taglib{,-devel} zlib gstreamer010{,-gst-plugins-base}
</pre>

### Build & Run Nulloy

<pre>
cd nulloy.git
./configure --taglib --no-gstreamer-tagreader --no-app-bundle --force-version 1-testing
make
./nulloy
</pre>

### Optional

Install extra GStreamer plugins for more audio formats

<pre>
sudo port install gstreamer010-gst-plugins-{good,bad,ugly}
</pre>



## Linux Build Instructions

### Installing Dependences

#### DEB-based distro

<pre>
apt-get install g++ libqt4-dev qt4-qmake libgstreamer{-plugins-base,}0.10-dev zip libx11-dev libtag1-dev
</pre>

#### RPM-based distro

<pre>
yum install gcc-c++ qt-devel gstreamer{-plugins-base,}-devel zip libX11-devel taglib-devel
</pre>

### Build & Run Nulloy

<pre>
cd nulloy.git
./configure --taglib --force-version 1-testing
make
./nulloy
</pre>

### Optional

Install extra GStreamer plugins for more audio formats

#### DEB-based distro

<pre>
apt-get install gstreamer0.10{-ffmpeg,-plugins-{good,bad,ugly}}
</pre>

#### RPM-based distro

<pre>
yum install gstreamer{-ffmpeg,-plugins-{good,bad,ugly}}
</pre>
