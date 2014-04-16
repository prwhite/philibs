#!/usr/bin/env make

## sdfont:
## 
## This will build a command line app, sdfont, that takes a single ttf file
## as an argument.  Output will be an SDF image and an associated plain-text
## font metric file.  
## 
## This is a clone of Lonesock's utility discussed and refined here:
## http://www.gamedev.net/topic/491938-signed-distance-bitmap-font-tool/
## I have pruned some unnecessary things from the original archive based
## on my project's needs.
## 
## Troubleshooting:
## 
## There are generally two things that can go wrong when building this:
## 
## 1) Freetype files cannot be found.  Install freetype2 and make sure
## 	headers and the library can be found by adjusting variables in this
## 	Makefile.
## 2) You don't have a compiler that likes the commands in the makefile.  E.g.,
## 	you don't have gcc/llvm installed and in your path, or you are on Windows.
## 	See what you can do about that.
## 
## To build, type the following:
## 	make sdfont

help:
	@fgrep -h "##" $(MAKEFILE_LIST) | fgrep -v fgrep | sed -e 's/\\$$//' | sed -e 's/##//'

TARGET := sdfont

DEPS +=	main.o
DEPS += BinPacker.o
DEPS += lodepng.o
DEPS += stb_image.o

CPPFLAGS += -I/usr/local/include/freetype2
LDLIBS += -lfreetype

LD := $(CXX)

$(TARGET): $(DEPS)
	$(CXX) $(LDFLAGS) $(DEPS) $(LDLIBS) -o $@

clean:
	rm $(DEPS) $(TARGET)
