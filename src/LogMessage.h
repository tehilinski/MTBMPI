/*! ----------------------------------------------------------------------------------------------------------
@file		LogMessage.h
@class		mtbmpi::LogMessage
@brief 		Class to format log messages consistently.
@details	Messages can be formatted as type **Message**, **Warning**, or **Error**.
		This class does not write the messages to output, rather it helps create them.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_LogMessage_h
#define INC_mtbmpi_LogMessage_h

#include <string>

namespace mtbmpi {

    class TaskID;

    class LogMessage
    {
	public:

	    /// Create an informational log message.
	    /// The input value for **msg** is replaced.
	    void Message ( std::string & msg );
	    void Message ( std::string & msg, TaskID const & taskID );
	    void Message ( std::string & msg, std::string const & taskIDstr );

	    /// Create a warning log message.
	    /// The input value for **msg** is replaced.
	    void Warning ( std::string & msg );
	    void Warning ( std::string & msg, TaskID const & taskID );
	    void Warning ( std::string & msg, std::string const & taskIDstr );

	    /// Create an error log message.
	    /// The input value for **msg** is replaced.
	    void Error ( std::string & msg );
	    void Error ( std::string & msg, TaskID const & taskID );
	    void Error ( std::string & msg, std::string const & taskIDstr );

	private:

	    /// @cond SKIP_PRIVATE

	    static std::string const warningPrefix;
	    static std::string const errorPrefix;

	    /// @endcond
    };

} // namespace mtbmpi

#endif // INC_mtbmpi_LogMessage_h
