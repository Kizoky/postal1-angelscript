#pragma once

#include "as_main.h"

#include "thing.h"

#include "weapon.h"
#include "Thing3d.h"
#include "character.h"
#include "doofus.h"
#include "dude.h"

char* AS_VarArgs(char* format, ...)
{
	va_list		argptr;
	static char		string[1024];

	va_start(argptr, format);
	_vsnprintf(string, sizeof(string), format, argptr);
	va_end(argptr);

	return string;
}

template<typename SELFCLASS, typename RETURNCLASS>
static void ThingCast(const char* selfname, const char* returnname, int& r, asIScriptEngine* engine)
{
	std::string opImplCast_rtrn = AS_VarArgs("%s opImplCast()", returnname);
	r = engine->RegisterObjectMethod(selfname, opImplCast_rtrn.c_str(), asFunctionPtr(asSELFCLASS_opImplCast_RETURNCLASS<RETURNCLASS, SELFCLASS>), asCALL_CDECL_OBJFIRST); assert(r >= 0); DumpObjectMethod(r);
	opImplCast_rtrn = AS_VarArgs("%s opImplCast()", selfname);
	r = engine->RegisterObjectMethod(returnname, opImplCast_rtrn.c_str(), asFunctionPtr(asSELFCLASS_opImplCast_RETURNCLASS<SELFCLASS, RETURNCLASS>), asCALL_CDECL_OBJFIRST); assert(r >= 0); DumpObjectMethod(r);

	std::string opCast_rtrn = AS_VarArgs("%s opCast()", returnname);
	r = engine->RegisterObjectMethod(selfname, opCast_rtrn.c_str(), asFunctionPtr(asSELFCLASS_opCast_RETURNCLASS<RETURNCLASS, SELFCLASS>), asCALL_CDECL_OBJFIRST); assert(r >= 0); DumpObjectMethod(r);
	opCast_rtrn = AS_VarArgs("%s opCast()", selfname);
	r = engine->RegisterObjectMethod(returnname, opCast_rtrn.c_str(), asFunctionPtr(asSELFCLASS_opCast_RETURNCLASS<SELFCLASS, RETURNCLASS>), asCALL_CDECL_OBJFIRST); assert(r >= 0); DumpObjectMethod(r);
}

template<typename SELFCLASS, typename RETURNCLASS>
static RETURNCLASS* asSELFCLASS_opCast_RETURNCLASS(SELFCLASS* self)
{
	RETURNCLASS* pointer = dynamic_cast<RETURNCLASS*>(self);
	if (pointer)
		return pointer;

	return nullptr;
}

template<typename SELFCLASS, typename RETURNCLASS>
static RETURNCLASS* asSELFCLASS_opImplCast_RETURNCLASS(SELFCLASS* self)
{
	RETURNCLASS* pointer = dynamic_cast<RETURNCLASS*>(self);
	if (pointer)
		return pointer;

	return nullptr;
}

// Registering vice-versa, also implicitly
#define AS_REGISTER_THING_CAST(SELF, RETURN) \
	ThingCast<SELF, RETURN>(#SELF, #RETURN, r, engine);

void CAngelScriptVM::RegisterCast(int& r)
{
	// Assuming class and AngelScript object type names are the same...
	AS_REGISTER_THING_CAST(CThing, CThing3d);
	AS_REGISTER_THING_CAST(CThing, CCharacter);
	AS_REGISTER_THING_CAST(CThing, CDude);
	AS_REGISTER_THING_CAST(CThing, CDoofus);
}