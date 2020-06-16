/*! ----------------------------------------------------------------------------------------------------------
@file		OutputMgr.h
@class		mtbmpi::OutputMgr
@brief 		Manages the output from tasks.
@details
		This is owned by the Blackboard, but is optional.
		Your application can inherit OutputMgr to provide
		the actual output functionality of OutputMgr::HandleOutputMessage.
		@see CommStrings::Receive as an example of retrieving a vector of packed strings.

		A class derived from OutputAdapterBase is the sink for output;
		In the child class implementation of HandleOutputMessage,
		output data from the MPI message will be written by the OutputAdapter.

		A No-Op OutputMgr is provided for when the OutputMgr is not used.
@example	../examples/OutputMgrExample.cpp
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_OutputMgr_h
#define INC_mtbmpi_OutputMgr_h

#include <mpi.h>
#include "OutputAdapterBase.h"
#include "OutputFactoryBase.h"
#include <memory>
#include <stdexcept>

namespace mtbmpi {

    class OutputMgr
    {
      public:

	typedef std::shared_ptr<OutputAdapterBase>	OutputAdapterPtr;
	typedef std::shared_ptr<OutputFactoryBase>	OutputFactoryPtr;

	/// Constructor
	OutputMgr (
	  OutputFactoryPtr useOutputFactory	///< pointer to OutputFactory
	  );	// here for doxygen bug

	/// Handle the output MPI message. Child class will implement.
	virtual void HandleOutputMessage (
	    MPI::Intracomm & comm,		///< MPI communicator with message
	    MPI::Status const & status );	///< Status from probe contains source ID and message tag

	OutputFactoryPtr GetOutputFactory ()
	{
	    return pOutputFactory;
	}

	virtual ~OutputMgr ();

      protected:

	OutputFactoryPtr pOutputFactory;	///< output sink factory
	OutputAdapterPtr pOutputAdapter;	///< sink for output; to be used by HandleOutputMessage

      private:

	/// @cond SKIP_PRIVATE

	// functions that should not be used; are not defined
	OutputMgr (OutputMgr const & object);
	OutputMgr & operator= (OutputMgr const & object);
	bool operator== (OutputMgr const & object) const;
	bool operator!= (OutputMgr const & object) const;

	/// @endcond
    };


    /// A No-Op OutputMgr
    class OutputMgr_NoOp : public OutputMgr
    {
      public:

	OutputMgr_NoOp ()
	: OutputMgr ( std::make_shared<OutputFactory_NoOp>() )
	{
	}

    };

} // namespace mtbmpi


#endif // INC_mtbmpi_OutputMgr_h
