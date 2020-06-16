/*------------------------------------------------------------------------------------------------------------
file		RunLogMgr.cpp
class		mtbmpi::RunLogMgr
brief 		Manages the log file.
project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#include "RunLogMgr.h"

#include <stdexcept>

namespace mtbmpi {


RunLogMgr::RunLogMgr (
    std::string const & logFileName)
    : fileName (logFileName)
{
    ofs.open ( logFileName.c_str(), std::ios::out | std::ios::trunc );
    if ( !ofs.is_open() )
    {
	std::string msg = "Error: could not open the file\n";
	msg += logFileName;
	msg += "\nfor output.";
	throw std::runtime_error (msg);
    }
}


} // namespace mtbmpi
