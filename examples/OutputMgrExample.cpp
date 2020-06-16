//------------------------------------------------------------------------------------------------------------
// File: OutputMgrExample.cpp
// Example of the MTBMPI MPI framework using the mtbmpi::OutputMgr class.
// Build:
//	mpicxx -I../src -o OutputMgrExample -g OutputMgrExample.cpp  ../build/cmake/libmtbmpi.debug.a
// Run:
//	mpiexec -n 4 ./OutputMgrExample
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

class WorkTask : public mtbmpi::TaskAdapterBase		// Does the work
{
  public:

    typedef mtbmpi::State	State;

    WorkTask (
      mtbmpi::Task & useParent,
      std::string const & taskName,
      StrVec const & cmdLineArgs)
      : mtbmpi::TaskAdapterBase (useParent, taskName, cmdLineArgs),
        className ( "WorkTask" )
    {
    }

  private:

    std::string const className;

    virtual State DoInitializeTask ()
    {
	State state = mtbmpi::State_Initialized;
	SendToLog( "WorkTask::DoInitializeTask" );
	return state;
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
	os << "results: ratio * id = " << ( ratio * parent.GetID() );
	SendToOutput( os.str() );

	state = mtbmpi::State_Completed;
	return state;
    }

    virtual State DoStopTask ()
    {
	State state = mtbmpi::State_Terminated;
	SendToLog( "WorkTask::DoStopTask" );
	return state;
    }

    virtual State DoPauseTask ()
    {
	State state = mtbmpi::State_Paused;
	SendToLog( "WorkTask::DoPauseTask" );
	return state;
    }

    virtual State DoResumeTask ()
    {
	State state = mtbmpi::State_Running;
	SendToLog( "WorkTask::DoResumeTask" );
	return state;
    }

    inline void SendToLog ( std::string const msg )
    {
	parent.SendMsgToLog ( msg );
    }

    void SendToOutput ( std::string const & msg )
    {
	static int const tag = mtbmpi::Tag_TaskResults;
	int const destID = GetParent().GetBlackboardID();

	std::string outputMsg = msg;
	mtbmpi::LogMessage logMsg;
	logMsg.Message( outputMsg, parent.GetID() );

	mtbmpi::comm.Send ( outputMsg.data(), outputMsg.size(), MPI::CHAR, destID, tag );
	mtbmpi::CheckErrorMPI( className );
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
//	Task Output
//------------------------------------------------------------------------------------------------------------

class OutputSink : public mtbmpi::OutputAdapterBase		// Writes output from tasks
{
  public:

    OutputSink (
	mtbmpi::OutputMgr & useParent)
    : mtbmpi::OutputAdapterBase (useParent)
    {
    }

    void Write ( std::string const & msg )
    {
	cout << msg << endl;
    }
};

class OutputFactory : public mtbmpi::OutputFactoryBase		// Makes OutputSink
{
  public:

    typedef mtbmpi::OutputFactoryBase::OutputAdapterPtr   OutputAdapterPtr;

    virtual OutputAdapterPtr Create (
	mtbmpi::OutputMgr & parent)
    {
	return OutputAdapterPtr ( new OutputSink (parent) );
    }
};

class OutputMgr : public mtbmpi::OutputMgr			// Receives output from tasks
{
  public:

    typedef mtbmpi::OutputMgr::OutputFactoryPtr 	OutputFactoryPtr;

    OutputMgr ()
    : mtbmpi::OutputMgr ( std::make_shared< OutputFactory >() ),
      pOutput ( dynamic_cast< OutputSink* >( pOutputAdapter.get() ) )
    {
    }

    virtual void HandleOutputMessage (
	MPI::Intracomm & comm,			// MPI communicator with message
	MPI::Status const & status )		// Status contains source ID and message tag
    {
	MPI::Status recvStatus;
	int const count = status.Get_count (MPI::CHAR);
	std::string buffer( count + 1, mtbmpi::NULL_CHAR );
	mtbmpi::comm.Recv( &buffer[0], count, MPI::CHAR, status.Get_source(), status.Get_tag(), recvStatus );
	std::string msg (buffer.begin(), buffer.begin() + count);
	pOutput->Write( msg );
    }

  private:

    OutputSink* pOutput;			// writes the output

};

//------------------------------------------------------------------------------------------------------------

class WorkMaster : public mtbmpi::Master
{
  public:

    typedef mtbmpi::Master::OutputMgrPtr	OutputMgrPtr;

    WorkMaster (
      int    argc,
      char** argv,
      TaskFactoryPtr     useTaskFactory,
      OutputMgrPtr       useOutputMgr,
      MpiCollectiveCBPtr mpiCollectiveCBPtr,
      std::string const  logFileName = "OutputMgrExample.log")
      : mtbmpi::Master (
	    argc, argv, 3, cout, useTaskFactory, useOutputMgr, mpiCollectiveCBPtr, logFileName )
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
    virtual void DoActionsBeforeTasks () {}		// Master does this before tasks created.
    virtual void DoActionsAtInitTasks () {}		// Master does this before task initialization.
    virtual void DoActionsBeforeTasksStart () {}	// Master does this before tasks are started.
    virtual void DoActionsAfterTasks ()  {}		// Master does this after tasks are stopped.
    virtual void DoActionsWhileActive () {}		// Master does this while action loop is active.
};

//------------------------------------------------------------------------------------------------------------

int main (int argc, char **argv)
{
    std::unique_ptr<WorkMaster> pMpiTask;
    try
    {
    	mtbmpi::Master::TaskFactoryPtr     pTaskFactory     ( new WorkTaskFactory() );
    	mtbmpi::Master::OutputMgrPtr       pOutputMgr       ( new OutputMgr() );
	mtbmpi::Master::MpiCollectiveCBPtr pMpiCollectiveCB ( new mtbmpi::MpiCollectiveCB_NoOp() );

 	pMpiTask = std::make_unique<WorkMaster>(
 			argc, argv, pTaskFactory, pOutputMgr, pMpiCollectiveCB );

	if ( pMpiTask->GetID() == mtbmpi::Master::GetBlackboardID() )	// Blackboard must be available
	{
	    cout << "\nOutputMgrExample: MTBMPI framework example of using OutputMgr." << endl;
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
