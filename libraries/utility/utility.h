#ifndef _ARDUINO_UTILITY_CPP_
#define _ARDUINO_UTILITY_CPP_

#define NULL 	0
#define MAX_DEBUG_MSG_SIZE 128

void PrintBytes(unsigned char *ucBuffer, int iBuflen);
bool StrnCmp(char *pString1, char *pString2, int iLen);
void debugPrintln(const char *format, ...);

/***********************************************************************************************/
/*! 
* \fn         :: debugPrintln()
* \author     :: Vignesh S
* \date       :: 06-FEB-2022
* \brief      :: This function prints debug strings
* \param[in]  :: string
* \return     :: NONE
*/
/***********************************************************************************************/
#ifdef PRINT_DEBUG
void debugPrintln(const char *format, ...)
{
	char arrcMsg[MAX_DEBUG_MSG_SIZE] = {0};
  va_list pArg;
  va_start(pArg, format);
  vsnprintf(arrcMsg, MAX_DEBUG_MSG_SIZE, format, pArg);
  va_end(pArg);    
  Serial.println(arrcMsg);
}
#endif

#endif // _ARDUINO_UTILITY_CPP_