/*------------------------------------------------------------------------------------------------------------
file		LogMessage.cpp
class		mtbmpi::LogMessage
brief 		Class to format log messages consistently.
details		Messages can be formatted as type **Message**, **Warning**, or **Error**.
		This class does not write the messages to output, rather it helps create them.
project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#include "LogMessage.h"
#include "TaskID.h"
#include "UtilitiesMPI.h"

namespace mtbmpi {

/// @cond SKIP_PRIVATE

std::string const LogMessage::warningPrefix = "Warning: ";
std::string const LogMessage::errorPrefix   = "ERROR: ";

//---------- Make message parts ----------

static inline void FormatTaskID ( std::string & taskIDstr )	// taskIDstr is replaced
{
    std::string text = "Tracker ID ";
    text += taskIDstr;
    text += ": ";
    taskIDstr = text;
}

static inline std::string FormatTaskID ( TaskID const & taskID )
{
    std::string idStr = ToString( taskID.GetID() );
    FormatTaskID( idStr );
    return idStr;
}


static inline std::string MakePrefix ( TaskID const & taskID )
{
    std::string text = DateTimeStampPrefix();
    text += FormatTaskID( taskID.GetID() );
    return text;
}

static inline std::string MakePrefix ( std::string const & taskIDstr )
{
    std::string text = DateTimeStampPrefix();
    std::string idStr = taskIDstr;
    FormatTaskID( idStr );
    text += idStr;
    return text;
}

/// @endcond

//--------- Make messages ----------

void LogMessage::Message ( std::string & msg )
{
    std::string text = DateTimeStampPrefix();
    text += msg;
    msg = text;
}

void LogMessage::Message ( std::string & msg, TaskID const & taskID )
{
    std::string text = MakePrefix( taskID );
    text += msg;
    msg = text;
}

void LogMessage::Message ( std::string & msg, std::string const & taskIDstr )
{
    std::string text = MakePrefix( taskIDstr );
    text += msg;
    msg = text;
}

void LogMessage::Warning ( std::string & msg )
{
    std::string text = DateTimeStampPrefix();
    text += warningPrefix;
    text += msg;
}

void LogMessage::Warning ( std::string & msg, TaskID const & taskID )
{
    std::string text = MakePrefix( taskID );
    text += warningPrefix;
    text += msg;
    msg = text;
}

void LogMessage::Warning ( std::string & msg, std::string const & taskIDstr )
{
    std::string text = MakePrefix( taskIDstr );
    text += warningPrefix;
    text += msg;
    msg = text;
}

void LogMessage::Error ( std::string & msg )
{
    std::string text = DateTimeStampPrefix();
    text += errorPrefix;
    text += msg;
    msg = text;
}

void LogMessage::Error ( std::string & msg, TaskID const& taskID )
{
    std::string text = MakePrefix( taskID );
    text += errorPrefix;
    text += msg;
    msg = text;
}

void LogMessage::Error ( std::string & msg, std::string const & taskIDstr )
{
    std::string text = MakePrefix( taskIDstr );
    text += errorPrefix;
    text += msg;
    msg = text;
}


} // namespace mtbmpi
