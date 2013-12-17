#ifndef _TIME_H_
#define _TIME_H_

/** Inits time module. */
void time_init(void);

/** Returns the current time, rounded to closest second. */
int time_get_s(void);

/** Returns the current second fraction, rounded to closest second. */
int time_get_us(void);

/** Sets current time.
 * @param [in] s The time, in seconds (rounded to lower integer).
 * @param [in] us The second fraction, in microsecond.
 */
void time_set(int s, int us);


#endif
