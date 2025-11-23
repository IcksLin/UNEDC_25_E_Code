#ifndef __GLOBAL_H
#define __GLOBAL_H
#include "stdio.h"

#define CLAMP(x, min, max) (((x) < (min)) ? (min) : ((x) > (max)) ? (max) \
                                                                  : (x))

extern float Yaw_copy;
extern float Pitch_copy;
extern float Roll_copy;

#endif
