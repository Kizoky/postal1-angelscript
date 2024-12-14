#include "as_main.h"

#include "doofus.h"

void CAngelScriptVM::RegisterCDoofus(int& r)
{
	r = engine->RegisterObjectType("CDoofus", sizeof(CDoofus), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<CDoofus>()); DumpObjectType(r, POSTAL_CDOOFUS);
}