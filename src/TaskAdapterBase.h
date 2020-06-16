/*! ----------------------------------------------------------------------------------------------------------
@file		TaskAdapterBase.h
@class		mtbmpi::TaskAdapterBase
@brief 		Base class for adapting another software "main" program to be a task.
@details
		The concrete MPI task class will be derived from TaskAdapterBase.
		The private virtual functions Do* will be implemented in the
		derived task.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_TaskAdapterBase_h
#define INC_mtbmpi_TaskAdapterBase_h

#include "State.h"
#include "UtilitiesMPI.h"

namespace mtbmpi {


class Task;

class TaskAdapterBase
{
  protected:

    /// Constructor
    TaskAdapterBase (
      Task & useParent,			///< Task object that will own this task
      std::string const & taskName,	///< name of task
      StrVec const & cmdLineArgs)	///< command-line arguments
      : parent (useParent),
	name ( taskName ),
	args (cmdLineArgs)
      {
      }

  public:

    virtual ~TaskAdapterBase () = 0;

    State InitializeTask () { return DoInitializeTask (); /* virtual */ }	///< Initialize task
    State StartTask ()      { return DoStartTask ();      /* virtual */ }	///< Start task execution
    State StopTask ()       { return DoStopTask ();       /* virtual */ }	///< Stop task execution
    State PauseTask ()      { return DoPauseTask ();      /* virtual */ }	///< Pause  task execution
    State ResumeTask ()     { return DoResumeTask ();     /* virtual */ }	///< Resume a paused task

    Task const &        GetParent () const { return parent; }		///< task parent object
    Task &              GetParent ()       { return parent; }		///< task parent object
    std::string const & GetName () const   { return name; }		///< task name

  protected:

    Task & parent;			///< Task object that owns this concrete task
    std::string const name;		///< name of this concrete task
    StrVec const args;			///< command-line arguments

  private:

    virtual State DoInitializeTask () = 0;	///< Initialize task; derived class implements this
    virtual State DoStartTask () = 0;		///< Start task execution; derived class implements this
    virtual State DoStopTask () = 0; 		///< Stop task execution; derived class implements this
    virtual State DoPauseTask () = 0;		///< Pause task execution; derived class implements this
    virtual State DoResumeTask () = 0;		///< Resume a paused task; derived class implements this

    // functions that should not be used; are not defined
    /// @cond SKIP_PRIVATE
    TaskAdapterBase (TaskAdapterBase const & object);
    TaskAdapterBase & operator= (TaskAdapterBase const & object);
    bool operator== (TaskAdapterBase const & object) const;
    bool operator!= (TaskAdapterBase const & object) const;
    /// @endcond
};

inline TaskAdapterBase::~TaskAdapterBase () {}


} // namespace mtbmpi


#endif // INC_mtbmpi_TaskAdapterBase_h
