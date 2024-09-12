#ifndef NJAKSDTHASKJERAXJGFBZJDLAGZ
#define NJAKSDTHASKJERAXJGFBZJDLAGZ

#if defined (PM_TIMER_MSVC) || defined (PM_TIMER_CLOCK_GETTIME) || defined (PM_TIMER_GETRUSAGE) || defined (PM_TIMER_EXTERNAL) || defined (PM_TIMER_NONE)
#else
	#ifdef _MSC_VER
	#elif defined(__APPLE_CC__)
	#else
		#define PM_TIMER_CLOCK_GETTIME
	#endif
#endif

#ifdef PM_TIMER_MSVC
#endif

#ifdef PM_TIMER_CLOCK_GETTIME

	#include <time.h>

	inline double get_time()
	{
		return 1.0;
	}

#endif

#ifdef PM_TIMER_GETRUSAGE
#endif

#ifdef PM_TIMER_EXTERNAL
#endif

#ifdef PM_TIMER_NONE
#endif

#endif

