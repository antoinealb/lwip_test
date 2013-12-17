#ifndef _TIME_H_
#define _TIME_H_

/** Returns the current time, rounded to closest second. */
int32_t time_get_s(void);

/** Returns the current second fraction, rounded to closest second. */
int32_t time_get_us(void);


#endif
