/*------------------------------------------------------------------------------------------------------------
file		LoggerMPI.cpp
class		mtbmpi::LoggerMPI
brief 		Provides logger functions with a consistent format and destination.
details		Entries are written to the log via MPI messages.
project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#include "mpi.h"
#include "LoggerMPI.h"
#include "ErrorHandling.h"
#include "UtilitiesMPI.h"

namespace mtbmpi {


// global communicator for tasks
extern MPI::Intracomm comm;


LoggerMPI::LoggerMPI (
    TaskID const blackboardID)
    : className ( "mtbmpi::LoggerMPI" ),
      idBlackboard ( blackboardID )
{
}

LoggerMPI::LoggerMPI (LoggerMPI const & rhs)
    : className ( rhs.className ),
      idBlackboard ( rhs.idBlackboard )
{
}

/// @cond SKIP_PRIVATE

void LoggerMPI::SendMsg ( std::string const & msg, MsgTags const tag )
{
    mtbmpi::comm.Send ( msg.data(), msg.size(), MPI::CHAR, idBlackboard.GetID(), tag );
    CheckErrorMPI( className );
}

/// @endcond

void LoggerMPI::Message ( std::string const & msg )
{
    std::string text = msg;
    logMsg.Message( text );
    SendMsg( text, Tag_LogMessage );
}

void LoggerMPI::Message ( std::string const & msg, TaskID const & taskID )
{
    std::string text = msg;
    logMsg.Message( text, taskID );
    SendMsg( text, Tag_LogMessage );
}

void LoggerMPI::Message ( std::string const & msg, std::string const & taskIDstr )
{
    std::string text = msg;
    logMsg.Message( text, taskIDstr );
    SendMsg( text, Tag_LogMessage );
}

void LoggerMPI::Warning ( std::string const & msg )
{
    std::string text = msg;
    logMsg.Warning( text );
    SendMsg( text, Tag_LogMessage );
}

void LoggerMPI::Warning ( std::string const & msg, TaskID const & taskID )
{
    std::string text = msg;
    logMsg.Warning( text, taskID );
    SendMsg( text, Tag_LogMessage );
}

void LoggerMPI::Warning ( std::string const & msg, std::string const & taskIDstr )
{
    std::string text = msg;
    logMsg.Warning( text, taskIDstr );
    SendMsg( text, Tag_LogMessage );
}

void LoggerMPI::Error ( std::string const & msg )
{
    std::string text = msg;
    logMsg.Error( text );
    SendMsg( text, Tag_ErrorMessage );
}

void LoggerMPI::Error ( std::string const & msg, TaskID const& taskID )
{
    std::string text = msg;
    logMsg.Error( text, taskID );
    SendMsg( text, Tag_ErrorMessage );
}

void LoggerMPI::Error ( std::string const & msg, std::string const & taskIDstr )
{
    std::string text = msg;
    logMsg.Error( text, taskIDstr );
    SendMsg( text, Tag_ErrorMessage );
}


} // namespace mtbmpi
