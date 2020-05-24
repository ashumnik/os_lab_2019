#include <string.h>

void RevertString(char *str)
{
	size_t length = strlen(str);
	for(size_t i = 0; i < (length >> 1); i++){
		char c = str[i];
		str[i] = str[length - i - 1];
		str[length - 1 - i] = c;	
	}	
}

