#include "as_main.h"
#include "character.h"

void CAngelScriptVM::RegisterCCharacter(int& r)
{
	AS_REGISTER_OBJ("CCharacter", CCharacter, POSTAL_CCHARACTER);

	AS_REGISTER_OBJMETHOD("CCharacter", "void UpdateFirePosition()", CCharacter, UpdateFirePosition);
}