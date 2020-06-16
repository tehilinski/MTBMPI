/*! ----------------------------------------------------------------------------------------------------------
@file		Blackboard.h
@class		mtbmpi::Blackboard
@brief		The Blackboard is the information broker for the concurrency processes.
@details
		Blackboard does not act upon any MPI processes, but can accept information
		from them. This object runs in its own MPI process.

		There are two output sinks: RunLogMgr and OutputMgr.
		RunLogMgr gets messages tagged as Tag_LogMessage and Tag_ErrorMessage.
		OutputMgr gets messages tagged as Tag_TaskResults.
		RunLogMgr is always created internally. OutputMgr is optional.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_Blackboard_h
#define INC_mtbmpi_Blackboard_h

#include "mpi.h"
#include "TaskID.h"
#include "SendsMsgsToLog.h"
#include "RunLogMgr.h"
#include "OutputMgr.h"
#include "timeutil.h"
#include <string>
#include <memory>

namespace mtbmpi {

    class Master;

    class Blackboard : public TaskID
    {
      public:

	typedef std::shared_ptr<RunLogMgr>		RunLogMgrPtr;
	typedef std::shared_ptr<OutputMgr>		OutputMgrPtr;

	/// Constructor
	Blackboard (
	  IDNum const myID,				///< MPI rank: my process
	  IDNum const controllerID,			///< rank:controller
	  OutputMgrPtr useOutputMgr,			///< output manager
	  std::string const & logFileNameRoot		///< optional log file name
	  );	// here for doxygen bug

	~Blackboard ();

	/// Start the main loop of the Blackboard.
	/// Wait for messages from tasks.
	/// Perform action according to type of message.
	void Activate ();

	RunLogMgr & GetRunLogMgr () const { return *pRunLogMgr; }	///< Get the logfile manager

	bool HaveOutputMgr () const { return pOutputMgr.get() != nullptr; }

	OutputMgrPtr GetOutputMgr () const { return pOutputMgr; }	///< Get the output manager

      private:

	/// @cond SKIP_PRIVATE

	std::string const defaultLogFileName;

	IDNum const idController;		// rank controller
	RunLogMgrPtr pRunLogMgr;
	OutputMgrPtr pOutputMgr;


	void Message ( std::string const & msg );

	void ReceiveAndLogMessage(
	  MPI::Status & status);		// status from Probe

	void ReceiveAndLogError (
	  MPI::Status & status);		// status from Probe

	std::string CreateLogFileName (
	  std::string const & logFileNameRoot);

	// functions that should not be used; are not defined
	Blackboard (Blackboard const & object);
	Blackboard & operator= (Blackboard const & object);
	bool operator== (Blackboard const & object) const;
	bool operator!= (Blackboard const & object) const;

	/// @endcond
    };

} // namespace mtbmpi

#endif // INC_mtbmpi_Blackboard_h
