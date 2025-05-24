#include "str_const.h"
str_const::str_const()
{
	STREAM = str_const::cvt("stream");
	num_tst = str_const::cvt("1234");



}
char* str_const::cvt(const char*str)
{
	char* it;
	it = (char *)malloc(strlen(str));
	strcpy(it, str);
	
	return it;
}
