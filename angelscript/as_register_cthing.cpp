#include "as_main.h"
#include "thing.h"

void CAngelScriptVM::RegisterCThing(int& r)
{
	r = engine->RegisterObjectType("CThing", sizeof(CThing), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<CThing>()); DumpObjectType(r, POSTAL_CTHING);
}