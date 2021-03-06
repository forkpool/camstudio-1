# CamStudio

![AppVeyor](https://ci.appveyor.com/api/projects/status/github/stevenhoving/camstudio?branch=master&svg=true)


## The fork
This is a personal fork of the camstudio source code found in the mecurial repository on sourceforge.net.
I have forked this project because:
* The installers that are availible on the web are not kosher.
* No mkv support out of the box.
* Flash based products are insecure.

## Goals
Besides creating a clean installer for camstudio I have found myself a couple of additional goals.
* Fix memory leaks - done.
* Remove unmaintainable features:
  * swf support - done.
  * xnote support - done.
  * update check - done.
  * video producer - parcial done.
* Upgrade project dependencies:
  * CxImage - pending.
  * zlib - done.
  * minilzo - done.
  * libconfig++ - done.
  * libpng - done.
* Replace static visual studio project files with CMake - done.
* Create release.
* Introduce unit test.
* Add support for mkv.
* Replace UI framework with Qt.
* Improve settings screen.

## Removed features
* swf support.
* xnote support.
* update check.

## Removed dependencies
* libxml.

## Prerequisites
* Git.
* Visual Studio 2017.
* Cmake 3.11.

## How to clone/build
1. `git clone https://github.com/stevenhoving/camstudio.git --recursive`
2. `bootstrap.cmd`
3. Now open the project in the `vs_build` directory and build it.

## The source forge project
https://sourceforge.net/p/camstudio

## Dependencies
State | Name | Project
----- | -----|--------
Official| libconfig | https://github.com/hyperrealm/libconfig
Official| minilzo | http://www.oberhumer.com/opensource/lzo/
Official| zlib | https://github.com/madler/zlib
Fork| CxImage | https://github.com/movableink/cximage

## Credits
A big thanks goes out to the original CamStudio developers. And to bestlily who had a initial port of CamStudio (https://github.com/bestlily/Camstudio), thanks.
