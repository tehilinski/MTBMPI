/*! ----------------------------------------------------------------------------------------------------------
@file		TaskFactoryBase.h
@class		mtbmpi::TaskFactoryBase
@brief 		Base class for a factory that creates a task.
@details	A task object created by this factory is derived from TaskAdapterBase.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_TaskFactoryBase_h
#define INC_mtbmpi_TaskFactoryBase_h

#include "UtilitiesMPI.h"
#include <memory>

namespace mtbmpi {

    class Task;
    class TaskAdapterBase;

    class TaskFactoryBase
    {
      public:

	typedef std::shared_ptr<TaskAdapterBase>	TaskAdapterPtr;

	virtual ~TaskFactoryBase () = 0;

	/// Create a task.
	virtual TaskAdapterPtr Create (
	  Task & parent,			///< Task object that will own this task
	  std::string const & taskName,		///< name of task
	  StrVec const & cmdLineArgs 		///< command-line arguments
	  ) = 0;

      private:

    };

    inline TaskFactoryBase::~TaskFactoryBase () {}

} // namespace mtbmpi


#endif // INC_mtbmpi_TaskFactoryBase_h
