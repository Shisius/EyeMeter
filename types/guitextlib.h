#ifndef _GUITEXTLIB_H_
#define _GUITEXTLIB_H_

#include "ai_types.h"
#include <string>
#include <map>

#define EYEGUI_TEXTMSG_MAXLEN 128

typedef enum
{
	EYEGUILANG_RUS = 0,
	_EYEGUILANG_MAX_
} EyeGuiLanguage;

/**
 * Fills error_text string by error text message depends on error bit word - lower bits have higher priority, only 1 error msg returns.
 * Also fills error_description by error description text to help the user with solving the problem. 
 * Returns True on success, False if language or error bit are not supported.
 */
bool eyeMeasResultErrorText(unsigned int error_word, unsigned int language, std::wstring & error_text, std::wstring & error_description);

#endif //_GUITEXTLIB_H_
