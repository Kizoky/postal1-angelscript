#include "as_main.h"

#include "Thing3d.h"

void CAngelScriptVM::RegisterCThing3d(int& r)
{
	r = engine->RegisterObjectType("CThing3d", sizeof(CThing3d), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<CThing3d>()); DumpObjectType(r, POSTAL_CTHING3D);
}