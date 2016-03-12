/**
 * Clément Nussbaumer, 10.03.2016
 * Methods copied from Wiring Pi project
 * wiringpi.com
 */

#include "localTime.h"
#include <sys/time.h>
#include <stdint.h>
#include <cstddef>

static uint64_t epochMilli, epochMicro;


/*
 * initialiseEpoch:
 *	Initialise our start-of-time variable to be the current unix
 *	time in milliseconds and microseconds.
 *********************************************************************************
 */

void initialiseEpoch(void) {
    struct timeval tv;

    gettimeofday(&tv, NULL);
    epochMilli = (uint64_t) tv.tv_sec * (uint64_t) 1000 + (uint64_t)(tv.tv_usec / 1000);
    epochMicro = (uint64_t) tv.tv_sec * (uint64_t) 1000000 + (uint64_t)(tv.tv_usec);
}

/*
 * millis:
 *	Return a number of milliseconds as an unsigned int.
 *********************************************************************************
 */

unsigned int millis()
{
    struct timeval tv ;
    uint64_t now ;

    gettimeofday (&tv, NULL) ;
    now  = (uint64_t)tv.tv_sec * (uint64_t)1000 + (uint64_t)(tv.tv_usec / 1000) ;

    return (uint32_t)(now - epochMilli) ;
}


/*
 * micros:
 *	Return a number of microseconds as an unsigned int.
 *********************************************************************************
 */

unsigned int micros()
{
    struct timeval tv ;
    uint64_t now ;

    gettimeofday (&tv, NULL) ;
    now  = (uint64_t)tv.tv_sec * (uint64_t)1000000 + (uint64_t)tv.tv_usec ;

    return (uint32_t)(now - epochMicro) ;
}
