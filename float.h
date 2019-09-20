#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

//Special values of float numbers, specify to use add
#define P_ZERO_F 0x0
#define N_ZERO_F 0x80000000
#define P_INF_F 0x7f800000
#define N_INF_F 0xff800000
#define P_NAN_F 0x7fc00000
#define N_NAN_F 0xffc00000

//sign macro to judge the sign of 32-bit number
#define sign(x) ((uint32_t)(x) >> 31)

//define bool type to use
typedef enum { false, true } bool;

//The float type struct, define fval in order to test
typedef union {
	struct
	{
		uint32_t fraction : 23;
		uint32_t exponent : 8;
		uint32_t sign : 1;
	};
	float fval;
	uint32_t val;
} FLOAT;

//define special situation to check
typedef struct
{
	uint32_t a;
	uint32_t b;
	uint32_t res;
} CORNER_CASE_RULE;

//core functions of adding two float numbers
uint32_t float_add(uint32_t a, uint32_t b);
