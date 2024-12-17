#ifndef POSTAL_ANGELSCRIPT_H
#define POSTAL_ANGELSCRIPT_H
#pragma once

#include "angelscript.h"
#include <string>
#include <vector>
#include "assert.h"

#include "thing.h"

// Only for dumping information!

struct ASGlobalFunctions_t { int index; };
struct ASMethods_t { int index; };
struct ASObjects_t
{
	int index;
	int identify;
};
struct ASProperties_t { int index; };
struct ASBehaviors_t { int index; };
struct ASGlobalProperties_t { int index; };

struct ASCachedObj_t
{
	asIScriptObject* obj;
	CThing* thing;
};

// Unique identifiers for debugging/misc.
enum
{
	// Base
	POSTAL_CTHING = 0,	// CThing, Derives from nothing
	 POSTAL_CANIMTHING,		// CAnimThing, Derives from CThing
	 POSTAL_CBALL,			// CBall, Derives from CThing
	 POSTAL_CBOUY,			// CBouy, Derives from CThing
	 POSTAL_CCHUNK,			// CChunk, Derives from CThing
	 POSTAL_CDEMON,			// CDemon, Derives from CThing
	 POSTAL_CDISPENSER,		// CDispenser, Derives from CThing
	 POSTAL_CEXPLODE,		// CExplode, Derives from CThing
	 POSTAL_CFIRE,			// CFire, Derives from CThing
	 POSTAL_CGAMEEDITTHING,	// CGameEditThing, Derives from CThing
	 POSTAL_CGOALTIMER,		// CGoalTimer, Derives from CThing
	 POSTAL_CHOOD,			// CHood, Derives from CThing
	 POSTAL_CLADDER,		// CLadder, Derives from CThing
	 POSTAL_CNAVIGATIONNET,	// CNavigationNet, Derives from CThing

	POSTAL_CWEAPON,		// CWeapon, Derives from CThing
	 POSTAL_CDEATHWAD,		// CDeathWad, Derives from CWeapon
	 POSTAL_CFIREBALL,		// CFireball, Derives from CWeapon
	 POSTAL_CFIREBOMB,		// CFirebomb, Derives from CWeapon
	 POSTAL_CGRENADE,		// CGrenade, Derives from CWeapon
	 POSTAL_CHEATSEEKER,	// CHeatseeker, Derives from CWeapon
	 POSTAL_CMINE,			// CMine, Derives from CWeapon
	 POSTAL_CNAPALM,		// CNapalm, Derives from CWeapon

	POSTAL_CTHING3D,	// CThing3d, Derives from CThing
	 POSTAL_CCHARACTER,		// CCharacter, Derives from CThing3d
	 POSTAL_CFLAG,			// CFlag, Derives from CThing3d
	 POSTAL_CFLAGBASE,		// CFlagbase, Derives from CThing3d
	 POSTAL_CITEM3D,		// CItem3d, Derives from CThing3d
	 
	POSTAL_CDUDE,		// CDude, Derives from CCharacter
	
	// Enemy base
	POSTAL_CDOOFUS,		// CDoofus, Derives from CCharacter

	 POSTAL_CPERSON,		// CPerson, Derives from CDoofus
	 POSTAL_CBAND,			// CBand, Derives from CDoofus 
	 POSTAL_CSENTRY,		// CSentry, Derives from CDoofus
	 POSTAL_COSTRICH,		// COstrich, Derives from CDoofus


};

enum
{
	LOG_NONE,
	LOG_SCRIPT, // Called by scripts

	LOG_CALLBACK,
	LOG_MAIN,
	LOG_COMPILE,

	LOG_OBJ_CONSTRUCT,
	LOG_OBJ_DESTRUCT,
	LOG_OBJ_FAIL
};

// Make the compiler not shriek
class CRealm;
class CDude;

class CAngelScriptVM
{
public:
	CAngelScriptVM();
	CAngelScriptVM(bool boot);
	int Setup(bool bRecompile);

public:
	// Global functions
	void RegisterGlobal(int& r);

	// Register casting
	// i.e. from CThing to CDude and vice versa...
	void RegisterCast(int& r);

	// Every object from the game is derived from CThing
	// this seems to be simple enough to grab pointers
	void RegisterCThing(int& r);

	// Derives from CThing
	void RegisterCThing3d(int& r);

	// Derives from CThing3D
	void RegisterCCharacter(int& r);

	void RegisterCDoofus(int& r);

	void RegisterCDude(int& r);
public:
	asIScriptEngine* GetEngine() { return engine; }
	asIScriptModule* GetModule() { return mod; }

	void Recompile();

public:
	asIScriptObject* CreateObj(std::string Class, CThing* thing, int id);
	void AddObjToCache(asIScriptObject* obj, CThing* thing);
	asIScriptObject* GetObjFromCache(CThing* thing);
	CThing* GetThingFromCache(asIScriptObject* obj);
	bool RemoveFromCache(asIScriptObject* obj, CThing* thing);

	CRealm* GetRealm();

	// maybe this isn't a good idea under multiplayer
	CDude* GetPlayer();
private:
	asIScriptEngine* engine;
	asIScriptModule* mod;

public:
	// Cached objects, destroyed on destruct
	std::vector<ASCachedObj_t> m_ASObjs;

public:
	// Contains the indexes of all Global functions registered
	std::vector< ASGlobalFunctions_t > m_ASGlobalFunctions;
	// Contains the indexes of all object type methods registered
	std::vector< ASMethods_t > m_ASMethods;
	// Contains the indexes of all object types registered
	std::vector< ASObjects_t > m_ASObjects;
	// Contains the indexes of all object type behaviors registered
	std::vector< ASBehaviors_t > m_ASBehaviors;
	// Contains the indexes of all global properties registered
	std::vector< ASGlobalProperties_t > m_ASGlobalProperties;
public:
	void AddLog(int logType, const char* str, ...);

	std::vector<std::string> GetLog() { return log; }
	void ClearLog() { log.clear(); }

	// Logs, console
	std::vector<std::string> log;

	bool bLogNone;
	bool bLogScript;

	bool bLogCallback;
	bool bLogMain;
	bool bLogCompile;

	bool bLogObjConstr;
	bool bLogObjDestr;
	bool bLogObjFail;
};

extern void DumpGlobalFunction(int index);
extern void DumpObjectType(int index, int identify);
extern void DumpObjectBehaviour(int index);
extern void DumpObjectMethod(int index);
extern void DumpGlobalProperty(int index);
extern void DumpInterface(int index);

// Accessor
extern CAngelScriptVM g_AngelScript;
extern std::string g_loaderPath;

// Registers an object. Normally in Script this will be name@
// Recommended to use this macro if object won't be a variable like Vector, QAngle, etc..
#define AS_REGISTER_OBJ( name, objclass, identifier ) \
	r = engine->RegisterObjectType(name, sizeof(objclass), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<objclass>() ); assert(r >= 0); DumpObjectType(r, identifier);
// Singleton registration
#define AS_REGISTER_OBJ_S( name ) \
	r = engine->RegisterObjectType(name, 0, asOBJ_REF | asOBJ_NOHANDLE); assert(r >= 0); DumpObjectType(r);
// Registers an interface
#define AS_REGISTER_INTERFACE( name ) \
	r = engine->RegisterInterface(name); assert(r >= 0); DumpInterface(r);
// Registers the method for the object. Only use this for functions that don't have strings in their params.
// Otherwise you'll need to use the F version.
#define AS_REGISTER_OBJMETHOD( obj, param, objclass, method ) \
	r = engine->RegisterObjectMethod(obj, param, asMETHOD(objclass, method), asCALL_THISCALL); assert(r >= 0); DumpObjectMethod(r);
// Registers the method for the object. Uses a static function for this. Object will always be first in param.
// Use this with a function like 'static void obj_MyFunc(CClass* self, ...)'
#define AS_REGISTER_OBJMETHOD_F( obj, param, method ) \
	r = engine->RegisterObjectMethod(obj, param, asFUNCTION(method), asCALL_CDECL_OBJFIRST); assert(r >= 0); DumpObjectMethod(r);
// Registers a property/variable for the object. Must use class variables. Avoid strings
#define AS_REGISTER_OBJVAR( obj, param, objclass, var ) \
	r = engine->RegisterObjectProperty(obj, param, asOFFSET(objclass, var));
// Registers a global function. Uses a static function.
#define AS_REGISTER_GLOBAL( param, function ) \
	r = engine->RegisterGlobalFunction(param, asFUNCTION(function), asCALL_CDECL); assert(r >= 0); DumpGlobalFunction(r);
// Registers a global function as generic. 
// Used for vararg functions. Uses a static function
#define AS_REGISTER_GLOBAL_G( param, function ) \
	r = engine->RegisterGlobalFunction(param, asFUNCTION(function), asCALL_GENERIC); assert(r >= 0); DumpGlobalFunction(r);
// Registers a global variable.
#define AS_REGISTER_GLOBALVAR( param, var ) \
	r = engine->RegisterGlobalProperty(param, var); assert(r >= 0); DumpGlobalProperty(r);

#endif // POSTAL_ANGELSCRIPT_H