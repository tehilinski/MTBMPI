//------------------------------------------------------------------------------------------------------------
// File: SimpleExample.cpp
// Simple example and test of the MTBMPI MPI framework.
// Build:
//	mpicxx -I../src -o SimpleExample -g SimpleExample.cpp  ../build/cmake/libmtbmpi.debug.a
// Run:
//	mpiexec -n 4 ./SimpleExample
//
// project	Master-Task-Blackboard MPI Framework
// author	Thomas E. Hilinski <https://github.com/tehilinski>
// copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
// 		This software library, including source code and documentation,
// 		is licensed under the Apache License version 2.0.
// 		See "LICENSE.md" for more information.
//------------------------------------------------------------------------------------------------------------

#include <iostream>
using std::cout;
using std::endl;
#include <exception>
#include <memory>

#include "MTBMPI.h"
using mtbmpi::StrVec;

//------------------------------------------------------------------------------------------------------------
//	Task
//------------------------------------------------------------------------------------------------------------

class WorkTask : public mtbmpi::TaskAdapterBase			// Does the work
{
  public:

    typedef mtbmpi::State		State;

    WorkTask (
      mtbmpi::Task & useParent,
      std::string const & taskName,
      StrVec const & cmdLineArgs)
      : mtbmpi::TaskAdapterBase (useParent, taskName, cmdLineArgs)
    {
    }

  private:

    virtual State DoInitializeTask ()
    {
	SendToLog( "WorkTask::DoInitializeTask" );
	return mtbmpi::State_Initialized;
    }

    virtual State DoStartTask ()
    {
	State state = mtbmpi::State_Running;

	SendToLog( "WorkTask::DoStartTask" );

	// do something time consuming
	float sumA = 0.0f;
	float sumI = 0.0f;
	long const limit = 1000000L;
	for ( long i = 0; i < limit; ++i )
	{
	    float a = i;
	    a = (a * a) / (float)(limit - 1.0f) + 0.5f;
	    sumA += a;
	    sumI += (float)i;
	}
	float const ratio = sumA / sumI;

	// try to make the tasks finish in order of their rank
	mtbmpi::Sleep( 1e5 * GetParent().GetID() );

	std::ostringstream os;
	os << "WorkTask: ratio * id = " << ( ratio * parent.GetID() );
	parent.SendMsgToLog ( os.str() );

	state = mtbmpi::State_Completed;
	return state;
    }

    virtual State DoStopTask ()
    {
	SendToLog( "WorkTask::DoStopTask" );
	return mtbmpi::State_Terminated;
    }

    virtual State DoPauseTask ()
    {
	SendToLog( "WorkTask::DoPauseTask" );
	return mtbmpi::State_Paused;
    }

    virtual State DoResumeTask ()
    {
	SendToLog( "WorkTask::DoResumeTask" );
	return mtbmpi::State_Running;
    }

    inline void SendToLog ( std::string const msg )
    {
	parent.SendMsgToLog ( msg );
    }
};

class WorkTaskFactory : public mtbmpi::TaskFactoryBase		// Makes WorkTask
{
  public:

    typedef mtbmpi::TaskFactoryBase::TaskAdapterPtr	TaskAdapterPtr;

    virtual TaskAdapterPtr Create (
      mtbmpi::Task & parent,
      std::string const & taskName,
      StrVec const & cmdLineArgs)
    {
	return TaskAdapterPtr ( new WorkTask (parent, taskName, cmdLineArgs) );
    }
};

//------------------------------------------------------------------------------------------------------------

class WorkMaster : public mtbmpi::Master
{
  public:

    WorkMaster (
      int    argc,
      char** argv,
      TaskFactoryPtr     useTaskFactory,
      MpiCollectiveCBPtr mpiCollectiveCBPtr,
      std::string const  logFileName = "SimpleExample.log")
      : mtbmpi::Master (
	    argc, argv, 3, cout,
	    useTaskFactory,					// makes Tasks
	    std::make_shared< mtbmpi::OutputMgr_NoOp >(),	// A no-op object; no output generated
	    mpiCollectiveCBPtr,					// callbacks after/before MPI init/term
	    logFileName )
    {
	Initialize ();
    }

  private:

    void Initialize ()
    {
	if ( GetID() == GetControllerID() )
	{
	    if ( IsInitialized() )
		GetController().Activate();	// event loop
	}
    }

    // Derived class actions called by controller
    virtual void DoActionsBeforeTasks ()		// Master does this before tasks created.
    {
	Log().Message( "WorkMaster::DoActionsBeforeTasks" );
    }

    virtual void DoActionsAtInitTasks ()		// Master does this before task initialization.
    {
	Log().Message( "WorkMaster::DoActionsAtInitTasks" );
    }

    virtual void DoActionsBeforeTasksStart ()		// Master does this before tasks are started.
    {
	Log().Message( "WorkMaster::DoActionsBeforeTasksStart" );
    }

    virtual void DoActionsAfterTasks ()			// Master does this after tasks are stopped.
    {
	Log().Message( "WorkMaster::DoActionsAfterTasks" );
    }

    virtual void DoActionsWhileActive ()		// Master does this while action loop is active.
    {
	Log().Message( "WorkMaster::DoActionsWhileActive" );
    }
};

//------------------------------------------------------------------------------------------------------------

class CallBacks : public mtbmpi::MpiCollectiveCB	// callbacks after/before MPI init/term
{
  public:

    CallBacks ()
    : mtbmpi::MpiCollectiveCB ()
    {
    }

    virtual void Initialize ()
    {
	Msg( std::string( "CallBacks::Initialize" ) );
    }

    virtual void Finalize ()
    {
	Msg( std::string( "CallBacks::Finalize" ) );
    }

  private:

    void Msg ( std::string const msg )
    {
	cout << "process " << GetID() << ": " << msg << endl;
    }
};

//------------------------------------------------------------------------------------------------------------

int main (int argc, char **argv)
{
    std::unique_ptr<WorkMaster> pMpiTask;
    try
    {
    	mtbmpi::Master::TaskFactoryPtr     pTaskFactory     ( new WorkTaskFactory() );
	mtbmpi::Master::MpiCollectiveCBPtr pMpiCollectiveCB ( new CallBacks() );

 	pMpiTask = std::make_unique<WorkMaster>( argc, argv, pTaskFactory, pMpiCollectiveCB );

	if ( pMpiTask->GetID() == mtbmpi::Master::GetBlackboardID() )	// Blackboard must be available
	{
	    cout << "\nSimpleExample: Demonstrates use of the MTBMPI framework." << endl;
	    cout << "Log file name: "
	         << pMpiTask->GetBlackboard().GetRunLogMgr().GetFileName()
	         << endl;
	    cout.flush();
	}
	else if ( pMpiTask->GetID() > mtbmpi::Master::GetBlackboardID() )
	    mtbmpi::Sleep( 100000 ); 	// try to force the intro from above to appear first

	cout << "process " << pMpiTask->GetID()
	     << ": started on CPU: " << mtbmpi::GetMPIProcessorName()
	     << endl;
    }
    catch (std::exception const & e)
    {
 	int const id = (pMpiTask.get() ? pMpiTask->GetID() : -1);
	cout << "main: rank = " << id
	     << "Exception: " << e.what() << endl;
    }
    catch (...)
    {
 	int const id = (pMpiTask.get() ? pMpiTask->GetID() : -1);
	cout << "main: rank = " << id
	     << "Unknown Exception" << endl;
    }

    cout << "process " << pMpiTask->GetID() << ": completed" << endl;

    if ( pMpiTask->GetID() == mtbmpi::Master::GetControllerID() )
    {
	mtbmpi::Sleep( 100000 ); 		// try to force this to appear last
	cout << "   all done!" << endl;
    }
    return 0;
}
