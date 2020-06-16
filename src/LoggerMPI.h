/*! ----------------------------------------------------------------------------------------------------------
@file		LoggerMPI.h
@class		mtbmpi::LoggerMPI
@brief 		Provides log messages with a consistent format and destination.
@details	A message is sent to Blackboard for the log file via MPI messaging.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_LoggerMPI_h
#define INC_mtbmpi_LoggerMPI_h

#include "TaskID.h"
#include "MsgTags.h"
#include "LogMessage.h"

namespace mtbmpi {

    class LoggerMPI
    {
	friend class SendsMsgsToLog;

      public:

	/// Constructor
	LoggerMPI (
	    TaskID const blackboardID		///< The Blackboard rank
	    );	// here for doxygen bug

	LoggerMPI (
	    LoggerMPI const & rhs);

	/// Write an informational message to the log file.
	void Message ( std::string const & msg );
	void Message ( std::string const & msg, TaskID const & taskID );
	void Message ( std::string const & msg, std::string const & taskIDstr );

	/// Write a warning message to the log file.
	void Warning ( std::string const & msg );
	void Warning ( std::string const & msg, TaskID const & taskID );
	void Warning ( std::string const & msg, std::string const & taskIDstr );

	/// Write an error message to the log file.
	void Error ( std::string const & msg );
	void Error ( std::string const & msg, TaskID const & taskID );
	void Error ( std::string const & msg, std::string const & taskIDstr );

      private:

	/// @cond SKIP_PRIVATE

	std::string const className;
	TaskID mutable idBlackboard;
	LogMessage logMsg;

	// Set the task ID number after MPI initialization is complete.
	void SetBbID ( TaskID::IDNum const blackboardID )
	{
	    idBlackboard.SetID( blackboardID );
	}

	void SendMsg ( std::string const & msg, MsgTags const tag );

	// functions not used
	LoggerMPI & operator= (LoggerMPI const & rhs);
	bool operator== (LoggerMPI const & rhs) const;
	bool operator!= (LoggerMPI const & rhs) const;

	/// @endcond
    };

} // namespace mtbmpi

#endif // INC_mtbmpi_LoggerMPI_h
