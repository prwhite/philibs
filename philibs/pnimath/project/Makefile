#########################################################################
### Copyright (c) 1999, 2000 Sony Computer Entertainment America Inc.
###	All rights reserved.
### 
### Boilerplate Makefile by Bret Mogilefsky (mogul@playstation.sony.com)
### 	with heaps of help from Tyler Daniel (tyler_daniel@playstation.sony.com)
###
###	Use this makefile as a template for new projects!
### 
### General Features:
###
###	Just specify SRCS and go!
###	Automatic and minimal (fast!) dependency generation (for vu microcode as well)
###	Allows keeping source and headers from src and include dirs, or elsewhere.
###	Builds in a subdirectory.
###	Allows additional defines, include dirs, and lib dirs without 
###		specifying -D, -I, and -L
###   Easy to specify parallel builds (debug, optimized, release, etc)
###   Easy to add flags on a per-file, per-build, or per-file-build basis
###	Can specify parent projects to make first (libraries)
###	Builds libraries
###	Slices, dices, feeds your cat, calls your mum.
###
### VU microcode features:
###
###	Generates depencies for microcode (for .include and #include)
###	Uses a preprocessing script to manage registers (configurable)
###	Runs the c preprocessor over microcode - you can use #define and #include
###		freely (and share #defines with c/c++)
###	Support for vcl
###
### Useful targets:
###
###	run		Run the executable.
###	xrun		Run the executable under a new xterminal.
###	clean		Remove everything we can rebuild.
###	tags		Generate source-browsing tags for Emacs.
###
### Using builds:
###
### 	To specify a particular build include the name of the build anywhere on
### 	the command line:
### 		make xrun optimized,
###   	make clean optimized, etc.
###
### 	Included builds (add your own!):
### 		debug
###		optimized	(default)
###		release
###
###	For more info see the "Build Options" section below
##########################################################################


##########################################################################
### Target
##########################################################################


# The name of the binary file we want to generate.  Also handles libraries! (.a)
TARGET		= libpnimath.so


##########################################################################
### Files and Paths - this is probably the only section you'll need to change
##########################################################################


# The source files for the project.

# .. or add them manually

SRCS		+= pnibox2.cpp
SRCS		+= pnibox3.cpp
SRCS		+= pnicylinder.cpp
SRCS		+= pnifloatmath.cpp
SRCS		+= pnifloattrait.cpp
SRCS		+= pnifrustum.cpp
SRCS		+= pnimath.cpp
SRCS		+= pnimathstream.cpp
SRCS		+= pnimatrix4.cpp
SRCS		+= pnimatstack.cpp
SRCS		+= pniplane.cpp
SRCS		+= pnipolytope.cpp
SRCS		+= pniquat.cpp
SRCS		+= pnirand.cpp
SRCS		+= pniseg.cpp
SRCS		+= pnisphere.cpp
SRCS		+= pnivec2.cpp
SRCS		+= pnivec3.cpp
SRCS		+= pnivec4.cpp

# [etc.]

# Additional objects to link. Only add things that aren't built from SRCS!
OBJS		= 

# Additional libs to link with. (sce libs are listed in the section below)
LIBS		=  pthread
#LIBS		+= $(EXTRA_LIBS)

# Additional locations for header files
INCDIRS		+= 

# Additional locations for library files
LIBDIRS		+= 

# Additional locations for source files
SRCDIRS		+= ../src

# Object files and the target will be placed in this directory with an
# underscore and the buildname appended (e.g., for the "debug" build: objs_debug/)
OBJDIRBASE	= ../lib/linux/objs

# Dependency files will be placed in this directory with an underscore and
# the buildname appended (e.g., for the "debug" build: deps_debug/)
DEPDIRBASE	= ../lib/linux/deps

# If this project depends other projects (a ps2 rendering library for example) that should
# be built with make before making this one, list the directories here.
MAKEPARENTS	= 

# Where to find PSX2 development stuff.
SCEDIR		= 

# Where to find the ps2stuff project
PS2STUFF	= 

##########################################################################
### Common Options (shared across builds)
##########################################################################

# link against libraries
#LIBS		+= libstdc++.a

LIBS += 

# find sce headers
#INCDIRS		+= /usr/include

# find sce libraries
LIBDIRS		+= /usr/lib /usr/X11R6/lib

# Additional preprocessor definitions
DEFINES		+= 

# Compiler optimization options
OPTFLAGS	+= 

# Compiler debug options

# enable all warnings
DEBUGFLAGS	+= -Wall 
#-Winline
# output assembly listings with c/c++ code
#DEBUGFLAGS	+= 
# -Wa,-alh
# This is not recommended as it generates slower code, but let's leave it
# as the default so that "*(u_long128*)&someVar" behaves as you expect.
# It would be better to remove this and not do the above (try templates).
#DEBUGFLAGS	+= -fno-strict-aliasing
# for multithreading to work properly?
#DEBUGFLAGS	+= -fno-common

# Command-line arguments to be passed to the target when we run it
RUNARGS		= 


##########################################################################
### Build Options - applied per-build
##########################################################################


# Which build to do if none is specified on the command line
DEFAULTBUILD		= optimized

################# application ####################

# Specifics for the "test" build
BUILDNAMES		+= test
test_DEBUGFLAGS	= -DDEBUG -DVERBOSE -DSTATS -g 
test_SRCS		= test.cpp

# Specifics for the "debug" build
BUILDNAMES		+= debug
debug_DEBUGFLAGS	= -DDEBUG -DVERBOSE -DSTATS -DLOGFILE -g 

# Specifics for the "logged" build
BUILDNAMES		+= logged
logged_DEBUGFLAGS	= -DLOGFILE
logged_OPTFLAGS		= -O3

# Specifics for the "profiler" build
BUILDNAMES		+= profiler
profiler_DEBUGFLAGS	= -pg 

# Specifics for the "optimized" build 
BUILDNAMES		+= optimized
optimized_DEBUGFLAGS	= 
optimized_OPTFLAGS	= -g -O3 -DNDEBUG

# Specifics for the "nonoptimized" build 
BUILDNAMES		+= nonoptimized
nonoptimized_DEBUGFLAGS	= -g

# Specifics for the "release" build
BUILDNAMES		+= release
release_OPTFLAGS	= -O3


# You can specify flags for a new build type "hamburger" as follows:
# BUILDNAMES		+= hamburger
# hamburger_INCDIRS	= someincdirs
# hamburger_LIBDIRS	= somelibdirs
# hamburger_DEFINES	= somedefs
# hamburger_OPTFLAGS	= someoptflags
# hamburger_DEBUGFLAGS	= somedebugflags
# hamburger_RUNARGS	= somerunargs


##########################################################################
### Per-file Options
##########################################################################


# Additional defines and include dirs can be specified on a per-file basis
# by prefixing with the stem of the filename.  For example, if I wanted special flags
# for building mucilage.cpp, I could add any of the following 
# mucilage_INCDIRS	= someincdirs 
# mucilage_LIBDIRS	= somelibdirs
# mucilage_DEFINES	= somedefs
# mucilage_OPTFLAGS	= someoptflags
# mucilage_DEBUGFLAGS	= somedebugflags


##########################################################################
### Per-file, per-build Options
##########################################################################


# Similar to above.. To apply special flags for building mucilage.cpp for
# the debug build, I could add any of the following
# mucilage_debug_INCDIRS	= someincdirs 
# mucilage_debug_LIBDIRS	= somelibdirs
# mucilage_debug_DEFINES	= somedefs
# mucilage_debug_OPTFLAGS	= someoptflags
# mucilage_debug_DEBUGFLAGS	= somedebugflags


##########################################################################
### Makefile operation
##########################################################################


# Set this to 1 to print status messages (like 'Compiling somefile.cpp...')
PRINT_MSGS		= 1

# Set this to 1 to print the exact command lines used to build files
PRINT_CMDS		= 0

##########################################################################
### include the makefile that does all the work
##########################################################################


include $(PNIUSR)/lib/Makefile.common
include $(PNIUSR)/lib/Makefile.work
