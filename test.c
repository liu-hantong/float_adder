#include "float.h"
int main()
{
	FLOAT a, b, result_correct, result_calculated;
	srand(time(NULL));
	for (uint32_t i = 1; i <= 100000000; ++i)
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

			printf("Failed! Check the outputs for more information!");
			break;
		}
	}
	return 0;
}
