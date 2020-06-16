/*! ----------------------------------------------------------------------------------------------------------
@file		OutputAdapterBase.h
@class		mtbmpi::OutputAdapterBase
@brief 		Base class for adapting the task's software output sink to
		accept output from via Blackboard's message passing mechanism.

		A No-Op OutputAdapter is provided for when the OutputMgr is not used.
@example	../examples/OutputMgrExample.cpp
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_OutputAdapterBase_h
#define INC_mtbmpi_OutputAdapterBase_h

namespace mtbmpi {

    class OutputMgr;

    class OutputAdapterBase
    {
      protected:

	/// Constructor
	OutputAdapterBase (
	    OutputMgr & useParent)	///< Output manager object that owns this output sink
	    : parent (useParent)
	    {
	    }

      public:

	virtual ~OutputAdapterBase () = 0;

      protected:

	OutputMgr & parent;

      private:

	/// @cond SKIP_PRIVATE

	// functions that should not be used; are not defined
	OutputAdapterBase (OutputAdapterBase const & object);
	OutputAdapterBase & operator= (OutputAdapterBase const & object);
	bool operator== (OutputAdapterBase const & object) const;
	bool operator!= (OutputAdapterBase const & object) const;

	/// @endcond
    };

    inline OutputAdapterBase::~OutputAdapterBase () {}



    /// A No-Op OutputAdapter
    class OutputAdapter_NoOp : public OutputAdapterBase
    {
      public:

	OutputAdapter_NoOp (
	    OutputMgr & useParent)	///< Output manager object that owns this output sink
	: OutputAdapterBase ( useParent )
	{
	}

    };

} // namespace mtbmpi

#endif // INC_mtbmpi_OutputAdapterBase_h
