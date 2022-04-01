# The Anarchic Kingdom
![image](https://user-images.githubusercontent.com/49120805/161201033-f16b5432-fa9d-4dbb-beca-3d547bd3584d.png)

This is a simple strategy game for MS-DOS. It runs on an 8088 machine with CGA graphics. If you just want to play it, you can find the pre-built binary package at http://dos.cyningstan.org.uk/downloads/64/the-anarchic-kingdom.
# Building Requirements
To build the project, you will need the following:
* OpenWatcom 2.0 C compiler and tools.
* The CGALIB graphics library - https://github.com/cyningstan/cgalib.
# To Build the Project
The following steps assume cross-compilation on a modern operating system. Both CGALIB and The Anarchic Kingdom can be built in DOS, but I'll leave you to figure out how to get the repositories on to such a system.
```
git clone https://github.com/cyningstan/anarchic
cd anarchic
git clone https://github.com/cyningstan/cgalib
wmake -f makefile.wcc
cd ..
wmake -f makefile.wcc
```
Once done, the ``anarchic`` directory will contain the built game, ready to be packaged or copied to whatever machine and directory you want to run it from.
