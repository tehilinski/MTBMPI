/*! ----------------------------------------------------------------------------------------------------------
@file		SendsMsgsToLog.h
@class		mtbmpi::SendsMsgsToLog
@brief 		Base class for objects which send messages to the log via Blackboard.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_SendsMsgsToLog_h
#define INC_mtbmpi_SendsMsgsToLog_h

#include "mpi.h"
#include "TaskID.h"
#include "LoggerMPI.h"

namespace mtbmpi {


// global communicator for tasks
extern MPI::Intracomm comm;


class SendsMsgsToLog : public TaskID
{
  public:

    IDNum GetBlackboardID () const { return idBlackboard.GetID(); }

    LoggerMPI & Log () { return logger; }	///< Get the logfile object

  protected:

    /// Constructor
    SendsMsgsToLog (
      IDNum const myID,				///< my rank
      IDNum const blackboardID)			///< blackboard rank
      : TaskID( myID ),
	idBlackboard ( blackboardID ),
	logger( blackboardID )
      {
      }

    /// Copy Constructor
    SendsMsgsToLog ( SendsMsgsToLog const & rhs )
      : TaskID( rhs.GetID() ),
	idBlackboard ( rhs.idBlackboard ),
	logger( rhs.logger )
      {
      }

    /// Set the IDs after MPI initialization is complete
    void SetIDs (
      IDNum const myID,				///< my rank
      IDNum const blackboardID )		///< blackboard rank
      {
	  SetID( myID );
	  logger.SetBbID( blackboardID );
      }

  public:

    virtual ~SendsMsgsToLog () = 0;

  private:

    /// @cond SKIP_PRIVATE

    TaskID mutable idBlackboard;
    LoggerMPI logger;

    // functions that should not be used; are not defined
    SendsMsgsToLog & operator= (SendsMsgsToLog const & rhs);
    bool operator== (SendsMsgsToLog const & rhs) const;
    bool operator!= (SendsMsgsToLog const & rhs) const;

    /// @endcond
};

inline SendsMsgsToLog::~SendsMsgsToLog () {}


} // namespace mtbmpi

#endif // INC_mtbmpi_SendsMsgsToLog_h
