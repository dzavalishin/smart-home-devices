


/* define F_CPU according to AVR_FREQ set in Makefile */
/* Is there a better way to pass such a parameter from Makefile to source code ? */
/*
#if   defined F3686400
#define F_CPU     3686400
#elif defined F7372800
#define F_CPU     7372800
#elif defined F8000000
#define F_CPU     8000000
#elif defined F14745600
#define F_CPU     14745600
#elif defined F16000000
#define F_CPU     16000000
#elif defined F20000000
#define F_CPU     20000000
#else
*/
/* if AVR frequency is not defined in Makefile, default to 16MHz */
#define F_CPU     16000000
//#define F_CPU     8000000

//#endif

#define F_OSC F_CPU

