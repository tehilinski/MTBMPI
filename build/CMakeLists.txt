# CMake configuration file
# project:      MTBMPI library
# Author:       Tom Hilinski <https://github.com/tehilinski>
# Date:         2020-05
# Build
#   debug:
#     cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX:PATH=../../test/install ..
#   release:
#     cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=/usr/local ..
#
# CMake reference: https://cmake.org/cmake/help/v3.10/index.html

cmake_minimum_required(VERSION 3.10)

set( appNameBase mtbmpi )

if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    SET ( TARGET_NAME ${appNameBase}.debug )
elseif ( CMAKE_BUILD_TYPE STREQUAL "Release" )
    SET ( TARGET_NAME ${appNameBase} )
endif ( CMAKE_BUILD_TYPE STREQUAL "Debug" )

project( "${TARGET_NAME}"
	 VERSION 2.0.0
	 DESCRIPTION "MTBMPI library"
	 LANGUAGES CXX )

set( CMAKE_SOURCE_DIR "${CMAKE_BINARY_DIR}/../../src" )
set( CMAKE_INSTALL_INCLUDEDIR "include/${appNameBase}")

#------------------------------------------- GNU compiler settings -------------------------------------------
#
set( CMAKE_C_FLAGS                  "-std=c++11" )
set( CMAKE_C_FLAGS_DEBUG            "-std=c++11 -O0 -ggdb3 -Wall" )
set( CMAKE_C_FLAGS_RELEASE          "-std=c++11 -O2" )
set( CMAKE_C_FLAGS_RELWITHDEBINFO   "-std=c++11 -O1 -g" )
set( CMAKE_CXX_FLAGS                "-std=c++11" )
set( CMAKE_CXX_FLAGS_DEBUG          "-std=c++11 -O0 -ggdb3 -Wall" )
#set( CMAKE_CXX_FLAGS_DEBUG          "-std=c++11 -O0 -ggdb3 -Wall -Wextra -Weffc++" )
set( CMAKE_CXX_FLAGS_RELEASE        "-std=c++11 -O2" )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-std=c++11 -O1 -g" )

#--------------------------------------------------- MPI -----------------------------------------------------

find_package( MPI REQUIRED )

#-------------------------------------------------- status ---------------------------------------------------

message( STATUS "build type:\t\t\t"        "${CMAKE_BUILD_TYPE}" )
message( STATUS "library file:\t\t"        "${TARGET_NAME}" )
message( STATUS "source directory:\t\t"    "${CMAKE_SOURCE_DIR}" )
message( STATUS "install path root:\t\t"   "${CMAKE_INSTALL_PREFIX}" )
message( STATUS "install path includes:\t" "${CMAKE_INSTALL_INCLUDEDIR}" )
message( STATUS "MPI include path:\t\t"    "${MPI_CXX_INCLUDE_DIRS}" )
message( STATUS "Run: ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS} EXECUTABLE ${MPIEXEC_POSTFLAGS} ARGS" )

#-------------------------------------------------- library --------------------------------------------------

set( SRCS_CPP
	../../src/Blackboard.cpp
	../../src/CommStrings.cpp
	../../src/Communicator.cpp
	../../src/Controller.cpp
	../../src/ErrorHandling.cpp
	../../src/LogMessage.cpp
	../../src/LoggerMPI.cpp
	../../src/Master.cpp
	../../src/OutputMgr.cpp
	../../src/RunLogMgr.cpp
	../../src/State.cpp
	../../src/Task.cpp
	../../src/timeutil.cpp
	../../src/Tracker.cpp
	../../src/UtilitiesMPI.cpp
	../../src/versionMTBMPI.cpp )

set( SRCS_H
	Blackboard.h
	CommStrings.h
	Communicator.h
	Configuration.h
	Controller.h
	ErrorHandling.h
	LogMessage.h
	LoggerMPI.h
	Master.h
	MpiCollectiveCB.h
	MTBMPI.h
	MsgTags.h
	OutputAdapterBase.h
	OutputFactoryBase.h
	OutputMgr.h
	RunLogMgr.h
	SendsMsgsToLog.h
	State.h
	TaskAdapterBase.h
	TaskFactoryBase.h
	Task.h
	TaskID.h
	TimerMPI.h
	timetypes.h
	timeutil.h
	Tracker.h
	UtilitiesMPI.h
	VersionData.h
	versionMTBMPI.h )

add_library( "${TARGET_NAME}" STATIC "${SRCS_CPP}" )

target_include_directories( "${TARGET_NAME}"
	PRIVATE "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>" )

target_include_directories( "${TARGET_NAME}" PRIVATE "${MPI_CXX_INCLUDE_DIRS}" )

# for tests:
# target_link_libraries( "${TARGET_NAME}" PUBLIC MPI::MPI_CXX )


# preprocessor
if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )

#      target_compile_definitions( "${TARGET_NAME}"
# 	PRIVATE -DDBG_MPI_MASTER
# 	PRIVATE -DDBG_MPI_BLACKBOARD
# 	PRIVATE -DDBG_MPI_CONTROLLER
# 	PRIVATE -DDBG_MPI_TASK
# 	PRIVATE -DDBG_MPI_COMMUNICATOR )

elseif ( CMAKE_BUILD_TYPE STREQUAL "Release" )

    target_compile_definitions( "${TARGET_NAME}"
	    PRIVATE -DNDEBUG )

endif ( CMAKE_BUILD_TYPE STREQUAL "Debug" )

#-------------------------------------------------- install --------------------------------------------------

include(GNUInstallDirs)

install( TARGETS "${TARGET_NAME}"
	 ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
         LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" )

install( DIRECTORY "${CMAKE_SOURCE_DIR}/."
	 DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	 FILES_MATCHING PATTERN "*.h" )

#---------------------------------------------------- end ----------------------------------------------------
