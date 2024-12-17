#include "as_main.h"
#include "thing.h"

void CAngelScriptVM::RegisterCThing(int& r)
{
	AS_REGISTER_OBJ("CThing", CThing, POSTAL_CTHING);
}