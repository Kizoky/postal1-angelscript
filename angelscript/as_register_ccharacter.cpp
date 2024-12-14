#include "as_main.h"
#include "character.h"

void CAngelScriptVM::RegisterCCharacter(int& r)
{
	r = engine->RegisterObjectType("CCharacter", sizeof(CCharacter), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<CCharacter>()); DumpObjectType(r, POSTAL_CCHARACTER);

	AS_REGISTER_OBJMETHOD("CCharacter", "void UpdateFirePosition()", CCharacter, UpdateFirePosition);
}