/*------------------------------------------------------------------------------------------------------------
file		Blackboard.cpp
class		mtbmpi::Blackboard
brief		The Blackboard is the information sink for the concurrency processes.
details
		Blackboard does not act upon any MPI processes, but can accept information
		from them. This object runs in its own MPI process.

		There are two output sinks: RunLogMgr and OutputMgr.
		RunLogMgr gets messages tagged as Tag_LogMessage and Tag_ErrorMessage.
		OutputMgr gets messages tagged as Tag_TaskResults.
		RunLogMgr is always created internally. OutputMgr is optional.

project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#include "Blackboard.h"
#include "Master.h"
#include "MsgTags.h"
#include "State.h"
#include "versionMTBMPI.h"
#include <algorithm>
#include "UtilitiesMPI.h"

// define the following to write diagnostics to std::cout
// #define DBG_MPI_BLACKBOARD

#ifdef DBG_MPI_BLACKBOARD
  #include <iostream>
  using std::cout;
  using std::endl;
#endif

namespace mtbmpi {


Blackboard::Blackboard (
    IDNum const myID,				// rank: my process
    IDNum const controllerID,			// rank:controller
    OutputMgrPtr useOutputMgr,			// output manager
    std::string const & logFileNameRoot)	// optional log file name
    : TaskID( myID ),
      defaultLogFileName ( versionMTBMPI.ProductNameShort() + std::string("_Log") ),
      idController (controllerID),
      pOutputMgr ( useOutputMgr )
{
    std::string const logFileName =
	( logFileNameRoot.empty() ?
	  CreateLogFileName( logFileNameRoot ) :
	  logFileNameRoot );
    #ifdef DBG_MPI_BLACKBOARD
	cout << "Blackboard log file: " << logFileName << endl;
    #endif
    pRunLogMgr.reset( new RunLogMgr (logFileName) );
    #ifdef DBG_MPI_BLACKBOARD
    // startup msg
    {
	std::string msg = "Log file started: ";
	msg += logFileName;
	GetRunLogMgr().Write( msg );
	GetRunLogMgr().Write( "Blackboard started." );
    }
    #endif

    // following is done by the creating object
    // Activate ();
}

Blackboard::~Blackboard ()
{
    // startup msg
    #ifdef DBG_MPI_BLACKBOARD
    {
	std::string msg = "Log file done.";
	GetRunLogMgr().Write( msg );
    }
    #endif

    pRunLogMgr.reset();
}

void Blackboard::Activate ()
{
    while (true)
    {
	// Wait for messages from tasks.
	// Perform action according to type of message.
	MPI::Status status;
	mtbmpi::comm.Probe ( MPI_ANY_SOURCE, MPI_ANY_TAG, status );
	switch ( status.Get_tag() )
	{
	  case Tag_TaskResults:
	  {
	    // send to output mgr to be retrieved and managed
	    if ( HaveOutputMgr() )
		GetOutputMgr()->HandleOutputMessage( mtbmpi::comm, status );
	    break;
	  }
	  case Tag_LogMessage:
	  {
	    ReceiveAndLogMessage(status);
	    break;
	  }
	  case Tag_ErrorMessage:
	  {
	    ReceiveAndLogError( status );
	    break;
	  }
	  case Tag_StopBlackboard:
	  case Tag_RequestStop:
	  case Tag_RequestStopTask:
	  {
	    #ifdef DBG_MPI_BLACKBOARD
	    cout << "Blackboard " << GetID() << ": "
		 << "Activate: Tag_RequestStop: enter" << endl;
	    #endif
	    // mark msg as received
	    mtbmpi::comm.Recv ( 0, 0, MPI::BYTE, status.Get_source(), status.Get_tag() );

	    // send confirmation
	    //GetRunLogMgr().Write( "Blackboard stopped.\n" );
	    Message( "Blackboard stopped.\n" );
	    mtbmpi::comm.Send ( 0, 0, MPI::BYTE, idController, Tag_Confirmation );

	    #ifdef DBG_MPI_BLACKBOARD
	    cout << "Blackboard " << GetID() << ": "
		 << "Activate: Tag_RequestStop: done"
		 << endl;
	    #endif
	    return;
	  }
	  default:
	  {
	    #ifdef DBG_MPI_BLACKBOARD
	    cout << "Blackboard " << GetID() << ": "
		 << "Activate: unhandled tag: " << status.Get_tag()
		 << endl;
	    #endif
	    break;
	  }
	}
    }
}

/// @cond SKIP_PRIVATE

void Blackboard::Message ( std::string const & msg )
{
    std::string text = DateTimeStampPrefix();
    text += msg;
    GetRunLogMgr().Write( text );
}

void Blackboard::ReceiveAndLogMessage (
    MPI::Status & status)		// status from Probe
{
    MPI::Status recvStatus;
    int const count = status.Get_count (MPI::CHAR);
    std::string buffer( count + 1, NULL_CHAR );
    mtbmpi::comm.Recv( &buffer[0], count, MPI::CHAR, status.Get_source(), status.Get_tag(), recvStatus );
    std::string msg (buffer.begin(), buffer.begin() + count);
    #ifdef DBG_MPI_BLACKBOARD
	cout << "Blackboard message: " << msg << endl;
    #endif
    GetRunLogMgr().Write( msg );
}

void Blackboard::ReceiveAndLogError (
    MPI::Status & status)		// status from Probe
{
    MPI::Status recvStatus;
    int const count = status.Get_count (MPI::CHAR);
    std::string buffer( count + 1, NULL_CHAR );
    mtbmpi::comm.Recv( &buffer[0], count, MPI::CHAR, status.Get_source(), status.Get_tag(), recvStatus );
    std::string msg;
    std::string const errorPrefix = "Error: ";
    if ( buffer.substr( 0, errorPrefix.size() ) != errorPrefix )
	msg = errorPrefix;
    msg.append (buffer.begin(), buffer.begin() + count);
    #ifdef DBG_MPI_BLACKBOARD
	cout << "Blackboard error message: " << msg << endl;
    #endif
    GetRunLogMgr().Write( msg );
}

std::string Blackboard::CreateLogFileName (
    std::string const & logFileNameRoot)
{
    // log file name has the form: logFileNameRoot.DATE.TIME.txt

    char const dot = '.';
    char const dash = '-';
    char const colon = ':';
    char const slash = '/';

    std::string date = DateStr();
    std::replace (date.begin(), date.end(), slash, dash);
    std::string time = TimeStr();
    std::replace (time.begin(), time.end(), colon, dash);

    std::string name = ( logFileNameRoot.empty() ? defaultLogFileName : logFileNameRoot );
    name += dot;
    name += date;
    name += dot;
    name += time;
    name += ".txt";
    return name;
}

/// @endcond


} // namespace mtbmpi
