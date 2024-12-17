#include "as_main.h"

#include "doofus.h"

void CAngelScriptVM::RegisterCDoofus(int& r)
{
	AS_REGISTER_OBJ("CDoofus", CDoofus, POSTAL_CDOOFUS);
}