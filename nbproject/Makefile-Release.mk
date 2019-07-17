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
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/gen-cpp/FriendServices.o \
	${OBJECTDIR}/gen-cpp/NewsFeedService.o \
	${OBJECTDIR}/gen-cpp/ProfileServices.o \
	${OBJECTDIR}/gen-cpp/SocialServices.o \
	${OBJECTDIR}/gen-cpp/SocialServices_constants.o \
	${OBJECTDIR}/gen-cpp/SocialServices_types.o \
	${OBJECTDIR}/http/Connection.o \
	${OBJECTDIR}/http/MultiplexedClient.o \
	${OBJECTDIR}/http/NoServicesInvokeHandler.o \
	${OBJECTDIR}/http/ProfileRequestHandler.o \
	${OBJECTDIR}/http/SubHTTPServer.o \
	${OBJECTDIR}/http/ZRequestHandler.o \
	${OBJECTDIR}/main.o


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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/zsocialhttp

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/zsocialhttp: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/zsocialhttp ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/gen-cpp/FriendServices.o: gen-cpp/FriendServices.cpp
	${MKDIR} -p ${OBJECTDIR}/gen-cpp
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/gen-cpp/FriendServices.o gen-cpp/FriendServices.cpp

${OBJECTDIR}/gen-cpp/NewsFeedService.o: gen-cpp/NewsFeedService.cpp
	${MKDIR} -p ${OBJECTDIR}/gen-cpp
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/gen-cpp/NewsFeedService.o gen-cpp/NewsFeedService.cpp

${OBJECTDIR}/gen-cpp/ProfileServices.o: gen-cpp/ProfileServices.cpp
	${MKDIR} -p ${OBJECTDIR}/gen-cpp
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/gen-cpp/ProfileServices.o gen-cpp/ProfileServices.cpp

${OBJECTDIR}/gen-cpp/SocialServices.o: gen-cpp/SocialServices.cpp
	${MKDIR} -p ${OBJECTDIR}/gen-cpp
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/gen-cpp/SocialServices.o gen-cpp/SocialServices.cpp

${OBJECTDIR}/gen-cpp/SocialServices_constants.o: gen-cpp/SocialServices_constants.cpp
	${MKDIR} -p ${OBJECTDIR}/gen-cpp
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/gen-cpp/SocialServices_constants.o gen-cpp/SocialServices_constants.cpp

${OBJECTDIR}/gen-cpp/SocialServices_types.o: gen-cpp/SocialServices_types.cpp
	${MKDIR} -p ${OBJECTDIR}/gen-cpp
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/gen-cpp/SocialServices_types.o gen-cpp/SocialServices_types.cpp

${OBJECTDIR}/http/Connection.o: http/Connection.cpp
	${MKDIR} -p ${OBJECTDIR}/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/http/Connection.o http/Connection.cpp

${OBJECTDIR}/http/MultiplexedClient.o: http/MultiplexedClient.cpp
	${MKDIR} -p ${OBJECTDIR}/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/http/MultiplexedClient.o http/MultiplexedClient.cpp

${OBJECTDIR}/http/NoServicesInvokeHandler.o: http/NoServicesInvokeHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/http/NoServicesInvokeHandler.o http/NoServicesInvokeHandler.cpp

${OBJECTDIR}/http/ProfileRequestHandler.o: http/ProfileRequestHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/http/ProfileRequestHandler.o http/ProfileRequestHandler.cpp

${OBJECTDIR}/http/SubHTTPServer.o: http/SubHTTPServer.cpp
	${MKDIR} -p ${OBJECTDIR}/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/http/SubHTTPServer.o http/SubHTTPServer.cpp

${OBJECTDIR}/http/ZRequestHandler.o: http/ZRequestHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/http/ZRequestHandler.o http/ZRequestHandler.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
