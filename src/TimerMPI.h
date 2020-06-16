/*! ----------------------------------------------------------------------------------------------------------
@file		TimerMPI.h
@class		mtbmpi::TimerMPI
@brief		?
@details
		Time is accumulated until reset.
		Time can be read while the timer is running.
		Time values are expressed in either floating-point seconds
		or integer clock ticks.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2013, 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_TimerMPI_h
#define INC_mtbmpi_TimerMPI_h

#include <mpi.h>

namespace mtbmpi
{

    class TimerMPI
    {
      public:

	/// constructor
	TimerMPI (
	    bool const startNow = false )	///< If true, start the timer immediately
	: running       ( false ),
	  total         ( 0 ),
	  intervalStart ( 0 ),
	  interval      ( 0 ),
	  tics          ( 0 )
	{
	    if ( startNow )
		start();
	}

	/// Set elapsed time to 0.0
	void reset ()
	{
	    running = false;
	    total = intervalStart = interval = 0.0;
	    tics = 0;
	}

	/// Start timing
	void start ()
	{
	    if ( !running )
	    {
		intervalStart = MPI::Wtime();
		running = true;
	    }
	}

	/// Stop timing
	double stop ()
	{
	    if ( running )
	    {
		update();
		running = false;
	    }
	    return total;
	}

	/// Read elapsed time (tics); resets interval; accumulates total
	double read_tics ()
	{
	    update();
	    return tics;
	}

	/// Read elapsed time (seconds); resets interval; accumulates total
	double read ()
	{
	    update();
	    return total;
	}

	/// Read last interval time (seconds); resets interval; accumulates total
	double read_interval ()
	{
	    update();
	    return interval;
	}

	/// True if clock is ticking
	bool is_running() const
	{
	    return running;
	}

	/// Is the MPI timer global to all processes?
	static bool is_MPI_global ()
	{
	    return MPI::WTIME_IS_GLOBAL;
	}

      private:

	/// @cond SKIP_PRIVATE

	bool running;
	double total;		// cumulative seconds
	double intervalStart;	// seconds at last call to MPI::Wtime
	double interval;	// seconds in last interval
	long tics;		// cumulative tics

	void update ()
	{
	    if ( running )
	    {
		double const timeNow = MPI::Wtime();
		interval = timeNow - intervalStart;
		total += interval;
		tics += interval / MPI::Wtick();
		intervalStart = timeNow;
	    }
	}

	/// @endcond
    };

} // namespace mtbmpi

#endif	// INC_mtbmpi_TimerMPI_h
