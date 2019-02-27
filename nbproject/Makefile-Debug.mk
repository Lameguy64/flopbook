#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Fl_CanvasClass.o \
	${OBJECTDIR}/Fl_ColorPalette.o \
	${OBJECTDIR}/FlopFrameClass.o \
	${OBJECTDIR}/FlopProjectClass.o \
	${OBJECTDIR}/dlg_newproject.o \
	${OBJECTDIR}/gif_export.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/mainui.o \
	${OBJECTDIR}/tables.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/C/fltk-1.3.4-1/lib -L/C/freeimage -lFreeImage -lfltk -lfltk_z -lole32 -lgdi32 -lcomctl32 -lcomdlg32 -luuid flopbookres.o

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/flopbook.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/flopbook.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/flopbook ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/Fl_CanvasClass.o: Fl_CanvasClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG -DWIN32 -I/C/fltk-1.3.4-1 -I/C/freeimage -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Fl_CanvasClass.o Fl_CanvasClass.cpp

${OBJECTDIR}/Fl_ColorPalette.o: Fl_ColorPalette.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG -DWIN32 -I/C/fltk-1.3.4-1 -I/C/freeimage -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Fl_ColorPalette.o Fl_ColorPalette.cpp

${OBJECTDIR}/FlopFrameClass.o: FlopFrameClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG -DWIN32 -I/C/fltk-1.3.4-1 -I/C/freeimage -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/FlopFrameClass.o FlopFrameClass.cpp

${OBJECTDIR}/FlopProjectClass.o: FlopProjectClass.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG -DWIN32 -I/C/fltk-1.3.4-1 -I/C/freeimage -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/FlopProjectClass.o FlopProjectClass.cpp

${OBJECTDIR}/dlg_newproject.o: dlg_newproject.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG -DWIN32 -I/C/fltk-1.3.4-1 -I/C/freeimage -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/dlg_newproject.o dlg_newproject.cpp

: flopbook.rc 
	@echo Performing Custom Build Step
	windres flopbook.rc flopbook.res

${OBJECTDIR}/gif_export.o: gif_export.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG -DWIN32 -I/C/fltk-1.3.4-1 -I/C/freeimage -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/gif_export.o gif_export.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG -DWIN32 -I/C/fltk-1.3.4-1 -I/C/freeimage -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/mainui.o: mainui.cxx 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG -DWIN32 -I/C/fltk-1.3.4-1 -I/C/freeimage -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mainui.o mainui.cxx

${OBJECTDIR}/tables.o: tables.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tables.o tables.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/flopbook.exe
	${RM} 

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
