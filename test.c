#include "float.h"
#define MAX_ADD_TIME 0xffffffff
int main()
{
	FLOAT a, b, result_correct, result_calculated;
	uint32_t i = 0;
	srand(time(NULL));
	for (; i < MAX_ADD_TIME; ++i)
	{
		uint32_t random1 = rand();
		uint32_t random2 = rand();
		uint32_t random3 = rand();
		uint32_t random4 = rand();

		//construct random number for two floats
		a.val = (random1 << 16) + random2;
		b.val = (random3 << 16) + random4; 

		//compare the result calculated by fpu
		//and the result calculated by my function
		result_correct.fval = a.fval + b.fval;
		result_calculated.val = float_add(a.val, b.val);

		if (result_calculated.val != result_correct.val)
		{
			//if wrong print two floats
			printf("%u+%u\n", a.val, b.val);
			printf("%f+%f\n", a.fval, b.fval);

			//print right and wrong results
			printf("%u ", result_correct.val);
			printf("%u\n", result_calculated.val);
			printf("%f ", result_correct.fval);
			printf("%f\n", result_calculated.fval);

			printf("\033[1;31mFailed!\033[0m Check the outputs for more information!");
			break;
		}
		if( i%(0xffff) == 0 && i != 0)
			printf("%u additions \033[0;32mpassed\033[0m.\n", i);
	}

	//finish test
	if( i == 0xffffffff)
   printf("\033[1;31mCongratulations!\033[0m All %u additions \033[0;32mpassed\033[0m.\n", MAX_ADD_TIME);

	return 0;
}
