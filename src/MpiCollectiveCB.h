/*! ----------------------------------------------------------------------------------------------------------
@file		MpiCollectiveCB.h
@class		mtbmpi::MpiCollectiveCB
@brief 		Base class that contains callback functions used at MPI initialization and termination.
@details
		These is a collective class -- all task processes will call these these callback functions.
		Initialize is called after MPI is initialized, and before the Blackboard is initialized.
		Finalize is called just before MPI is finalized, and after other tasks are stopped, and
		after the Blackboard is destroyed.
		Since the Blackboard is not available, so no messages can be sent to the log file.
		Likewise, no messages can be sent or received from other MPI tasks.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_MpiCollectiveCB_h
#define INC_mtbmpi_MpiCollectiveCB_h

#include "TaskID.h"

class Master;

namespace mtbmpi {

    class MpiCollectiveCB : public TaskID
    {
	friend class Master;

      public:

	/// Function called by all processes immediately after MPI initialization.
	virtual void Initialize () = 0;

	/// Function called by all processes immediately before MPI finalization.
	virtual void Finalize () = 0;

      protected:

	MpiCollectiveCB ()
	: TaskID ( -1 )
	{
	}

      public:

	virtual ~MpiCollectiveCB () = 0;

      private:

	/// @cond SKIP_PRIVATE

	// functions that should not be used
	MpiCollectiveCB & operator= (MpiCollectiveCB const & object);
	bool operator== (MpiCollectiveCB const & object) const;
	bool operator!= (MpiCollectiveCB const & object) const;

	/// @endcond
    };

    inline MpiCollectiveCB::~MpiCollectiveCB () {}


    /// Implement MpiCollectiveCB with no-op callbacks.
    class MpiCollectiveCB_NoOp : public MpiCollectiveCB
    {
      public:
	MpiCollectiveCB_NoOp ()
	  : MpiCollectiveCB()
	  {}
	virtual void Initialize () {}
	virtual void Finalize ()   {}
    };

} // namespace mtbmpi

#endif // INC_mtbmpi_MpiCollectiveCB_h
