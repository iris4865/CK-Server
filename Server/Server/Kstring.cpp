#include "Kstring.h"

int Kstring::compare(const char* first,const char* second)
{
	int firstSize = sizeof(first);
	int secondSize = sizeof(second);

	if (firstSize < secondSize)
		return -1;
	else if (firstSize > secondSize)
		return 1;
	else
	{
		for (auto i = 0; i < firstSize; i++)
		{
			if (first[i] < second[i])
				return -1;
			else if (first[i] > second[i])
				return 1;
		}
	}
	return 0;
}

bool Kstring::equal(const char * first, const char * second)
{
	int firstSize = sizeof(first);
	int secondSize = sizeof(second);

	if (firstSize != secondSize)
		return false;
	else
	{
		for (auto i = 0; i < firstSize; i++)
		{
			if (first[i] < second[i])
				return false;
		}
	}
	return true;
}
