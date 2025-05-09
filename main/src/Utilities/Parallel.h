/** ***********************************************************************************************
* @file			Parallel.h
* @brief		This file contains includes and switches for multithreaded and SIMD parallelization
* @details		Details:
* 				- allows to simply switch between different modes of parallelization
*
* @author		Gerstmayr Johannes
* @date			2022-01-13 (created)
* @copyright	This file is part of Exudyn. Exudyn is free software: you can redistribute it and/or modify it under the terms of the Exudyn license. See 'LICENSE.txt' for more details.
* @note			Bug reports, support and further information:
* 				- email: johannes.gerstmayr@uibk.ac.at
* 				- weblink: https://github.com/jgerstmayr/EXUDYN
* 				
*
************************************************************************************************ */

#ifndef PARALLEL__H
#define PARALLEL__H

#include "Utilities/BasicDefinitions.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//multithreaded support
#ifdef USE_NGSOLVE_TASKMANAGER
	#include "Linalg/Use_avx.h" //include before NGsolve includes!!
	typedef size_t NGSsizeType;
	#include "ngs-core-master/ngs_core.hpp"
#else
	#include "Linalg/Use_avx.h" 
typedef Index NGSsizeType;
//replace used functions with dummy functions
namespace ngstd {
	template <typename TFUNC>
	inline void ParallelFor(NGSsizeType n, TFUNC f, int tasks_per_thread = 1, NGSsizeType costs = 1000)
	{
		for (Index i = 0; i < n; i++)
		{
			f(i);
		}
	}
	inline int  EnterTaskManager() { return 1; };
	inline void ExitTaskManager(int num_threads) {};

	class TaskManager
	{
	public:
		static bool IsRunning() { return false; }
		static int GetNumThreads() { return 1; }
		static void SetNumThreads(int amax_threads) {};
		static int GetMaxThreads() { return 1; }
		static int GetThreadId() { return 0; }
		static void SuspendWorkers(Index asleep_usecs = 1000) {}
		static void ResumeWorkers() {} // { sleep = false; }
	};
	typedef Index SizeType;
	typedef SizeType TotalCosts;
};

#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "Utilities/Threading.h"

#ifdef USE_MICROTHREADING//if this is defined, use MicroThreading instead? of NGsolve taskmanager
namespace exuThreading = MicroThreading; //this works better for smaller systems
#else
namespace exuThreading = ngstd; //NGsolve; this works better for larger systems; much better for contact!
#endif

#endif //PARALLEL__H

