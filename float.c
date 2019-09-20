#include "float.h"

// the last three bits of the significand are reserved for the GRS bits
uint32_t normalize(uint32_t sign, int32_t exp, uint64_t sig_grs)
{

	// normalization
	bool overflow = false; // true if the result is INFINITY or 0 during normalize

	if ((sig_grs >> (23 + 3)) > 1 || exp < 0)
	{
		uint32_t sticky = sig_grs & 0x1;
		// normalize toward right
		while (((sig_grs >> (23 + 3)) > 1) && exp < 0xff)
		{
			// shift right and pay attention to sticker bit
			sticky = sticky | (sig_grs & 0x1);
			sig_grs = sig_grs >> 1;
			sig_grs |= sticky;
			exp++;
		}

		if (exp >= 0xff)
		{
			// assign the number to infinity
			exp = 0xff;
			sig_grs = 0;	
			overflow = true;
		}
	}
	else if (((sig_grs >> (23 + 3)) == 0) && exp > 0)
	{
		// normalize toward left
		while (((sig_grs >> (23 + 3)) == 0) && exp > 0)
		{
			// shift left
			sig_grs = sig_grs << 1;
			exp--;
		}
		if (exp == 0)
		{
			// denormal
			// shift right, pay attention to sticky bit
			if((sig_grs & 0x1) == 1)
			{
				sig_grs = sig_grs >> 1;
				// keep the sticker
				sig_grs = sig_grs | 0x1;
			}
			else if((sig_grs & 0x1) == 0)
			{
				sig_grs = sig_grs >> 1;
				sig_grs = sig_grs & 0xfffffffe;
			}
		}
	}
	else if (exp == 0 && sig_grs >> (23 + 3) == 1)
	{
		// two denormals result in a normal
		exp++;
	}
	
	if (!overflow)
	{
		// round up and remove the GRS bits
		// add the GRS to the left part
		uint32_t GRS = sig_grs & 0x7;
		uint32_t closeGRS = (sig_grs & 0x8) >> 3;

		if(GRS == 4 && closeGRS == 1)
			sig_grs += 8;
		else if (GRS == 4 && closeGRS == 0)
			sig_grs = sig_grs + 0;
		else if (GRS > 4)
			sig_grs += 8;
		else if (GRS < 4)
			sig_grs = sig_grs + 0;
		// move and erase grs
		sig_grs = sig_grs >> 3;
		
		if((sig_grs >> (23)) > 1)
		{
			// shift right
			sig_grs = sig_grs >> 1;
			exp++;
			if((exp & 0xff) == 0xff)
				overflow = true;
		}
		
		// cut the highest bit
		sig_grs = sig_grs & 0x7fffff;
	}

	FLOAT f;
	f.sign = sign;
	f.exponent = (uint32_t)(exp & 0xff);
	f.fraction = sig_grs; // only the lowest 23 bits are kept
	return f.val;
}

CORNER_CASE_RULE corner_add[] = {
	{P_ZERO_F, P_ZERO_F, P_ZERO_F},
	{N_ZERO_F, P_ZERO_F, P_ZERO_F},
	{P_ZERO_F, N_ZERO_F, P_ZERO_F},
	{N_ZERO_F, N_ZERO_F, N_ZERO_F},
	
	{P_INF_F, N_INF_F, N_NAN_F},
	{N_INF_F, P_INF_F, N_NAN_F}
};

bool isNaN(uint32_t float_number)
{
	//exponent all ones and fraction not all zeros
	if (((float_number&0x7f800000) == 0x7f800000) && ((float_number&0x7fffff) != 0))
		return true;
	else
		return false;
}

bool isINF(uint32_t float_number)
{
	//exponent all ones and fraction all zeros
	if (((float_number&0x7f800000) == 0x7f800000) && ((float_number&0x7fffff) == 0))
		return true;
	else
		return false;
}

// a + b
uint32_t float_add(uint32_t a, uint32_t b)
{
	/* corner cases, do disnormal judges */
	// corner cases 1, including ZERO and INF additions
	int i = 0;
	for (; i < sizeof(corner_add) / sizeof(CORNER_CASE_RULE); i++)
	{
		if (a == corner_add[i].a && b == corner_add[i].b)
			return corner_add[i].res;
	}

	// corner cases 2, one is NaN, another is INF
	if (isINF(a) && isNaN(b))
		return (b | (uint32_t)0x400000);

	else if (isINF(b) && isNaN(a))
		return (a | (uint32_t)0x400000);


	// corner cases 3, one is ZERO, return another
	if (a == P_ZERO_F || a == N_ZERO_F)
	{
		if (isNaN(b))
			return (b | (uint32_t)0x400000);
	}

	if (b == P_ZERO_F || b == N_ZERO_F)
	{
		if (isNaN(a))
			return (a | (uint32_t)0x400000);
	}

	//definition of two additors
	FLOAT f, fa, fb;
	fa.val = a;
	fb.val = b;

	// corner cases 4, one of the number is infity or NaN, return this number
	if (fb.exponent == 0xff)
	{
		if (isNaN(fb.val))
			fb.val|= 0x400000;
		return fb.val;
	}

	if (fa.exponent == 0xff)
	{
		if (isNaN(fa.val))
			fa.val |= 0x400000;
		return fa.val;
	}

	/* non-corner cases, do normal additions */
	if (fa.exponent > fb.exponent)
	{
		fa.val = b;
		fb.val = a;
	}

	uint32_t sig_a, sig_b, sig_res;
	sig_a = fa.fraction;
	if (fa.exponent != 0)
		sig_a |= 0x800000; // the hidden 1
	sig_b = fb.fraction;
	if (fb.exponent != 0)
		sig_b |= 0x800000; // the hidden 1

	// alignment shift for fa
	uint32_t shift = 0;
	shift = (fb.exponent == 0 ? fb.exponent + 1 : fb.exponent) - (fa.exponent == 0 ? fa.exponent + 1 : fa.exponent);

	sig_a = (sig_a << 3); // guard, round, sticky
	sig_b = (sig_b << 3);

	uint32_t sticky = 0;
	while (shift > 0)
	{
		sticky = sticky | (sig_a & 0x1);
		sig_a = sig_a >> 1;
		sig_a |= sticky;
		shift--;
	}

	// fraction add
	if (fa.sign)
	{
		sig_a *= -1;
	}
	if (fb.sign)
	{
		sig_b *= -1;
	}

	sig_res = sig_a + sig_b;

	if (sign(sig_res))
	{
		f.sign = 1;
		sig_res *= -1;
	}
	else
	{
		f.sign = 0;
	}

	uint32_t exp_res = fb.exponent;
	
	// normalize the result and return
	return normalize(f.sign, exp_res, sig_res);
}
