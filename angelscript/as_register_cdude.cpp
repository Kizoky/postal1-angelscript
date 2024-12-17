#include "as_main.h"
#include "dude.h"

void CAngelScriptVM::RegisterCDude(int& r)
{
	AS_REGISTER_OBJ("CDude", CDude, POSTAL_CDUDE);
}