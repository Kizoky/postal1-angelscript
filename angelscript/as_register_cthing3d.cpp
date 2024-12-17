#include "as_main.h"

#include "Thing3d.h"

void CAngelScriptVM::RegisterCThing3d(int& r)
{
	AS_REGISTER_OBJ("CThing3d", CThing3d, POSTAL_CTHING3D);
}