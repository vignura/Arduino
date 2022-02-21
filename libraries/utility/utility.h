#ifndef _ARDUINO_UTILITY_CPP_
#define _ARDUINO_UTILITY_CPP_

#define NULL 	0

void PrintBytes(unsigned char *ucBuffer, int iBuflen);
bool StrnCmp(char *pString1, char *pString2, int iLen);
void debug_println(const char *format, ...);

#endif // _ARDUINO_UTILITY_CPP_