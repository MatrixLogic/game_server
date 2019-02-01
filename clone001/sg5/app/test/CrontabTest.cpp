#include "LogicInc.h"
#include <stdio.h>
int main()
{
	FILE *f1;
	f1 = fopen("/data/release/sgonline/s1/webroot/data/aa","w+");
	if(!f1)
	{
		cout << "open err" << endl;
	}
	fprintf(f1, "Hello, crotab!\n\r");
	fclose(f1);

	return 0;
}
