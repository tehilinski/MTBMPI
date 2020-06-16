/*! ----------------------------------------------------------------------------------------------------------
@file		OutputFactoryBase.h
@class		mtbmpi::OutputFactoryBase
@brief 		Base class for a factory that creates an output sink.
@details	An output objects are derived from OutputAdapterBase and are produced by
		the Create method.

		A no-op OutputFactory is provided for when the OutputMgr is not used.
@example	../examples/OutputMgrExample.cpp
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_OutputFactoryBase_h
#define INC_mtbmpi_OutputFactoryBase_h

#include <memory>

namespace mtbmpi {

    class OutputMgr;
    class OutputAdapterBase;

    class OutputFactoryBase
    {
      public:

	typedef std::shared_ptr<OutputAdapterBase>		OutputAdapterPtr;

	virtual ~OutputFactoryBase () = 0;

	/// Create an output manager
	virtual OutputAdapterPtr Create (
	  OutputMgr & parent		///< output manager object that will own this output sink
	  ) = 0;

      private:

    };

    inline OutputFactoryBase::~OutputFactoryBase () {}



    /// A No-Op OutputFactory
    class OutputFactory_NoOp : public OutputFactoryBase
    {
      public:

	OutputFactory_NoOp ()
	: OutputFactoryBase ()
	{
	}

	virtual OutputAdapterPtr Create (
	    OutputMgr & parent)
	{
	    return std::make_shared<OutputAdapter_NoOp>( parent );
	}
    };

} // namespace mtbmpi

#endif // INC_mtbmpi_OutputFactoryBase_h
