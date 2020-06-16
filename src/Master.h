/*! ----------------------------------------------------------------------------------------------------------
@file		Master.h
@class		mtbmpi::Master
@brief		This is the main class for starting and ending a MPI job.
		Implements a master/task pattern with a blackboard for work process' communication.
@details
		The Master runs in process with MPI rank == 0
		and owns the controller and configuration objects.
		The process with rank == ID_Blackboard has a Master owning a
		Blackboard but not a Controller.
		Other ranks > ID_Blackboard have a Master, that owns a Task,
		that owns a concrete TaskAdapterBase (the application's work task.)

		Your application must have its own master object which inherits mtbmpi::Master
		and implements the virtual private methods ``DoActions*``.

		The derived Master class should activate the Controller in its constructor,
		so that derived class can do things after this base class constructor but
		before the Controller's event loop is running:
		`pController->Activate();`
		The constructor can have code like this:
@code
		if ( GetID() == GetControllerID() )
		{
		    if ( IsInitialized() )	// mtbmpi::Master is initialized?
		    {
			// other derived class initialization here... then:
			GetController().Activate();	// event loop
		    }
		}
@endcode
@example	../examples/SimpleExample.cpp
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_Master_h
#define INC_mtbmpi_Master_h

#include "SendsMsgsToLog.h"
#include "Configuration.h"
#include "Controller.h"
#include "Blackboard.h"
#include "MpiCollectiveCB.h"
#include "ErrorHandling.h"
#include <memory>
#include <iosfwd>

namespace mtbmpi {


extern MpiCollectiveCB_NoOp const null_MpiCollectiveCB;


class Master : public SendsMsgsToLog
{
  public:

    typedef std::shared_ptr<mtbmpi::TaskFactoryBase>	TaskFactoryPtr;
    typedef std::shared_ptr<OutputMgr>			OutputMgrPtr;
    typedef std::shared_ptr<mtbmpi::MpiCollectiveCB>	MpiCollectiveCBPtr;
    typedef std::shared_ptr<mtbmpi::Configuration>	ConfigurationPtr;
    typedef std::shared_ptr<mtbmpi::Controller>		ControllerPtr;
    typedef std::shared_ptr<mtbmpi::Blackboard>		BlackboardPtr;
    typedef std::shared_ptr<mtbmpi::Task>		TaskPtr;
    typedef std::pair<int const, char const * const *>	ArgPair;		///< command-line  arc, argv

  protected:

    /// Constructor; used only by derived class
    Master (
      int const argc,					///< number of arguments in argv
      char const * const * const argv,			///< arguments
      IDNum const useMinNumProc,			///< minimum number of MPI processes expected
      std::ostream & msgStream,				///< output stream to receive messages
      TaskFactoryPtr useTaskFactory,			///< pointer to task factory object
      OutputMgrPtr useOutputMgr,			///< output manager (can be empty)
      MpiCollectiveCBPtr mpiCollectiveCBPtr,		///< pointer to object with callbacks
      std::string const logFileName			///< name of log file
	 = EMPTY_STRING_STATIC);

  public:

    /// Master (MPI rank == 0) does this before tasks created.
    void ActionsBeforeTasks () { DoActionsBeforeTasks (); }

    /// Master (MPI rank == 0) does this before task initialization.
    void ActionsAtInitTasks () { DoActionsAtInitTasks (); }

    /// Master (MPI rank == 0) does this before tasks are started.
    void ActionsBeforeTasksStart ()  { DoActionsBeforeTasksStart(); }

    /// Master (MPI rank == 0) does this while action loop is active.
    void ActionsWhileActive () { DoActionsWhileActive (); }

    /// Master (MPI rank == 0) does this after tasks are stopped.
    void ActionsAfterTasks ()  { DoActionsAfterTasks (); }

    ArgPair const &     GetArgs ()   const { return argPair; }		///< get command-line argc, argv

    int GetNumberOfProcesses () const        { return numProc; }	///< no. of MPI processes
    int GetMinimumNumberOfProcesses () const { return minNumProc; }	///< minimum no. of MPI processes

    Configuration const & GetConfiguration () const { return *pConfig; }	///< Get configuration

    static IDNum GetControllerID ()	{ return ID_Master; }		///< MPI rank of controller task
    static IDNum GetBlackboardID ()	{ return ID_Blackboard; }	///< MPI rank of blackboard task
    static IDNum GetFirstTaskID ()	{ return ID_FirstTask; }	///< MPI rank of 1st work task

    /// Check if the Master is initialized
    bool IsInitialized () const;

    /// No check for existence! Only rank = 0 has this.
    Controller & GetController () const { return *pController; }

    /// No check for existence! Only rank = 1 has this.
    Blackboard & GetBlackboard () const { return *pBlackboard; }

    /// Is the task ID a valid value?
    bool IsValidTaskID (IDNum const id) { return (id >= ID_FirstTask && id < numProc); }

    virtual ~Master () = 0;

  protected:

    static const IDNum ID_Master     = 0;	///< rank of Controller task
    static const IDNum ID_Blackboard = 1;	///< rank of Blackboard task
    static const IDNum ID_FirstTask  = 2;	///< rank of 1st work task

    void WaitUntilStopped ();			///< Wait until the Controller is stopped.

    void StopAllTasks ();			///< Tell Controller to stop all tasks

    /// Tell Controller to stop Blackboard
    void StopBlackboard ()
      {
	if ( pController.get() )
	    pController->StopBlackboard();
      }

    std::ostream & MsgStream () { return os; }		///< Get the message output stream

  private:

    /// @cond SKIP_PRIVATE

    int numProc;				// number of processes
    int minNumProc;				// minimum number of processes
    ConfigurationPtr pConfig;			// Configuration
    ControllerPtr    pController;		// Controller
    BlackboardPtr    pBlackboard;		// Blackboard
    TaskPtr          pTask;			// Task
    TaskFactoryPtr   pTaskFactory;		// task factory
    MpiCollectiveCBPtr pMpiCollectiveCB;	// MPI collective callback functions object
    ArgPair argPair;				// arc, argv
    std::ostream & os;				// output stream for messages

    void CreateProcesses (
      std::string const & logFileName,
      OutputMgrPtr useOutputMgr );

    void MakeTask (
      IDNum const id);		// mpi task rank

    /// @endcond

    // Derived class tasks upon controller activation
    virtual void DoActionsBeforeTasks () = 0;		///< Called before tasks created.
    virtual void DoActionsAtInitTasks () = 0;		///< Called before task initialization.
    virtual void DoActionsBeforeTasksStart () = 0;	///< Called after initialization and before task runs.
    virtual void DoActionsWhileActive () = 0;		///< Called this while action loop is active.
    virtual void DoActionsAfterTasks ()  = 0;		///< Called after tasks are stopped.

    // functions that should not be used; are not defined
    /// @cond SKIP_PRIVATE
    Master (Master const & object);
    Master & operator= (Master const & object);
    bool operator== (Master const & object) const;
    bool operator!= (Master const & object) const;
    /// @endcond

};


} // namespace mtbmpi


#endif // INC_mtbmpi_Master_h
