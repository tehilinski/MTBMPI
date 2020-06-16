/*! ----------------------------------------------------------------------------------------------------------
@file		TaskID.h
@class		mtbmpi::TaskID
@brief 		Owns and manages a task or MPI::rank ID number.
@details
		Task IDs are zero-based. A negative ID is one that is not specified. @n
		This class can either be inherited or be a class or function object.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_TaskID_h
#define INC_mtbmpi_TaskID_h

namespace mtbmpi {

    class TaskID
    {
	public:

	    typedef int	IDNum;	///< type for an ID number; a negative ID specifies an invalid ID

	    /// Constructor
	    TaskID (
		IDNum const taskID )		///< task rank
	    : id ( taskID )
	    {
	    }

	    /// Get the ID number
	    IDNum GetID () const
	    {
		return id;
	    }

	    ///  Set the ID number
	    void SetID ( IDNum const newID )
	    {
		id = newID;
	    }

	    /// true if ID is valid else false
	    bool IsValid () const
	    {
		return id >= 0;
	    }

	    virtual ~TaskID () {}

	private:

	    /// @cond SKIP_PRIVATE

	    IDNum mutable id;

	    /// @endcond
    };

} // namespace mtbmpi

#endif // INC_mtbmpi_TaskID_h
