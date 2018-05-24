/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ar19937.h                                                                  */
/* 24.05.2018                                                                 */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - these are the controlling constants for the ar19937 pseudo-random number */
/*   generator                                                                */
/*                                                                            */
/******************************************************************************/

#ifndef _AR19937_H_
#define _AR19937_H_

/******************************************************************************/

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A   0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

/******************************************************************************/

extern void          init_genrand(unsigned long s);
extern unsigned long genrand_int32(void);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/