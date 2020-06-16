/*! ----------------------------------------------------------------------------------------------------------
@file		RunLogMgr.h
@class		mtbmpi::RunLogMgr
@brief 		Manages the log file.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_RunLogMgr_h
#define INC_mtbmpi_RunLogMgr_h

#include <string>
#include <fstream>

namespace mtbmpi {


class RunLogMgr
{
  public:

    /// Constructor
    RunLogMgr (
      std::string const & logFileName			///< name of log file
      );	// here for doxygen bug

    ~RunLogMgr ()
      {
	Close();
      }

    /// Is the log file open for writing?
    bool IsOpen () { return ofs.is_open(); }

    /// Write a message to the log file.
    void Write ( std::string const & msg )
      {
	if ( ofs.is_open() )
	  ofs << msg << std::endl;
      }

    /// Close the log file.
    void Close ()
      {
	if ( ofs.is_open() )
	    ofs.close();
      }

    /// Get the log file name
    std::string const & GetFileName () const { return fileName; }

  private:

    /// @cond SKIP_PRIVATE

    std::string const fileName;		// name of log file
    std::ofstream ofs;			// output file stream

    // functions that should not be used; are not defined
    RunLogMgr (RunLogMgr const & object);
    RunLogMgr & operator= (RunLogMgr const & object);
    bool operator== (RunLogMgr const & object) const;
    bool operator!= (RunLogMgr const & object) const;

    /// @endcond
};


} // namespace mtbmpi


#endif // INC_mtbmpi_RunLogMgr_h
