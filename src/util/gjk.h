#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//----------------------------------------------------------------------------- 
// Gilbert-Johnson-Keerthi (GJK) collision detection algorithm in 2D 
// http://www.dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/ 
// http://mollyrocket.com/849 
//----------------------------------------------------------------------------- 

struct _vec2 { float x; float y; }; 
typedef struct _vec2 vec2; 

int gjk (const vec2* vertices1, size_t count1, const vec2* vertices2, size_t count2);

#ifdef __cplusplus
}
#endif
