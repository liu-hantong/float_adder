#include "float.h"
int main()
{
	FLOAT a, b, resultCorrect, resultCalculated;
	srand(time(NULL));
	for (uint32_t i = 1; i <= 0xffffffff; ++i)
	{
		uint32_t random1 = rand();
		uint32_t random2 = rand();
		uint32_t random3 = rand();
		uint32_t random4 = rand();
		a.val = (random1 << 16) + random2;
		b.val = (random3 << 16) + random4; 

		resultCorrect.fval = a.fval + b.fval;
		resultCalculated.val = float_add(a.val, b.val);

		if (resultCalculated.val != resultCorrect.val)
		{
			printf("%u+%u\n", a.val, b.val);
			printf("%f+%f\n", a.fval, b.fval);
			printf("%u ", resultCorrect.val);
			printf("%u\n", resultCalculated.val);
			printf("%f\n", resultCorrect.fval);
			printf("%f\n", resultCalculated.fval);
			printf("Failed!");
			break;
		}
	}
	system("pause");
	return 0;
}