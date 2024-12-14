#include "angelscript.h"

#include "as_main.h"
#include "assert.h"

#include "add_on/scriptstdstring/scriptstdstring.h"
#include "add_on/scriptarray/scriptarray.h"
#include "add_on/scriptdictionary/scriptdictionary.h"
#include "add_on/scripthandle/scripthandle.h"
#include "add_on/scriptany/scriptany.h"
#include "add_on/weakref/weakref.h"

#include "add_on/scriptbuilder/scriptbuilder.h"

#include <iostream>
#include <windows.h>

#include <cstdarg>

#include "add_on/contextmgr/contextmgr.h"
#include "add_on/debugger/debugger.h"

#include <vector>

CAngelScriptVM g_AngelScript(true);

CContextMgr* g_ctxMgr = 0;
std::vector<asIScriptContext*> g_ctxPool;
CDebugger* g_dbg = 0;

asIScriptContext* RequestContextCallback(asIScriptEngine* engine, void* param);
void              ReturnContextCallback(asIScriptEngine* engine, asIScriptContext* ctx, void* param);

extern void DumpGlobalFunction(int index)
{
	ASGlobalFunctions_t temp;
	temp.index = index;

	g_AngelScript.m_ASGlobalFunctions.push_back(temp);
}

extern void DumpObjectType(int index, int identify)
{
	ASObjects_t temp;
	temp.index = index;
	temp.identify = identify;

	g_AngelScript.m_ASObjects.push_back(temp);
}

extern void DumpObjectBehaviour(int index)
{
	ASBehaviors_t temp;
	temp.index = index;

	g_AngelScript.m_ASBehaviors.push_back(temp);
}

extern void DumpObjectMethod(int index)
{
	ASMethods_t temp;
	temp.index = index;

	g_AngelScript.m_ASMethods.push_back(temp);
}

extern void DumpGlobalProperty(int index)
{
	ASGlobalProperties_t temp;
	temp.index = index;

	g_AngelScript.m_ASGlobalProperties.push_back(temp);
}

extern void DumpInterface(int index)
{

}

// This function is called by the engine whenever a context is needed for an 
// execution we use it to pool contexts and to attach the debugger if needed.
asIScriptContext* RequestContextCallback(asIScriptEngine* engine, void* /*param*/)
{
	asIScriptContext* ctx = 0;

	// Check if there is a free context available in the pool
	if (g_ctxPool.size())
	{
		ctx = g_ctxPool.back();
		g_ctxPool.pop_back();
	}
	else
	{
		// No free context was available so we'll have to create a new one
		ctx = engine->CreateContext();
	}

	// Attach the debugger if needed
	if (ctx && g_dbg)
	{
		// Set the line callback for the debugging
		ctx->SetLineCallback(asMETHOD(CDebugger, LineCallback), g_dbg, asCALL_THISCALL);
	}

	return ctx;
}

// This function is called by the engine when the context is no longer in use
void ReturnContextCallback(asIScriptEngine* engine, asIScriptContext* ctx, void* /*param*/)
{
	// We can also check for possible script exceptions here if so desired

	// Unprepare the context to free any objects it may still hold (e.g. return value)
	// This must be done before making the context available for re-use, as the clean
	// up may trigger other script executions, e.g. if a destructor needs to call a function.
	ctx->Unprepare();

	// Place the context into the pool for when it will be needed again
	g_ctxPool.push_back(ctx);
}

CAngelScriptVM::CAngelScriptVM()
{
	engine = nullptr;
	mod = nullptr;
	log.clear();
}

// TODO: I'm sorry penguins
std::string GetCurrentDir()
{
	char buffer[MAX_PATH];
	DWORD length = GetModuleFileName(NULL, buffer, MAX_PATH);
	if (length == 0)
	{
		throw std::runtime_error("Failed to get executable path");
	}
	std::string fullpath = std::string(buffer, length);

	size_t pos = fullpath.find_last_of("/\\"); // Find the last slash or backslash
	if (pos == std::string::npos)
	{
		throw std::runtime_error("Failed to extract directory path");
	}
	return fullpath.substr(0, pos); // Return the path up to the last slash
}

static std::string g_loaderPath;

CAngelScriptVM::CAngelScriptVM(bool boot)
{
	g_loaderPath = GetCurrentDir() + std::string("/scripts/angelscript.txt");
	Setup(false);
}

void CAngelScriptVM::AddLog(const char* str, ...)
{
	va_list		argptr;
	char		string[1024];
	va_start(argptr, str);
	_vsnprintf(string, sizeof(string), str, argptr);
	va_end(argptr);

	string[1024 - 1] = 0;
	std::string newLog = string;
	log.push_back(newLog);
}

// Implement a simple message callback function
void MessageCallback(const asSMessageInfo* msg, void* param)
{
	const char* type = "ERR";
	if (msg->type == asMSGTYPE_WARNING)
	{
		type = "WARN";
	}
	else if (msg->type == asMSGTYPE_INFORMATION)
	{
		type = "INFO";
	}

	g_AngelScript.AddLog("%s (L: %d) : %s : %s", msg->section, msg->row, type, msg->message);
}

// 1 == success
// 0 == error
// -1 == catastrophic
int CAngelScriptVM::Setup(bool bRecompile)
{
	g_AngelScript.AddLog("Setting up AngelScript %s", ANGELSCRIPT_VERSION_STRING);

	int r = 0;

	if (!bRecompile)
	{
		engine = asCreateScriptEngine(); assert(engine == nullptr);
		mod = GetEngine()->GetModule("PostalModule", asGM_CREATE_IF_NOT_EXISTS); assert(mod == nullptr);

		r = GetEngine()->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL); assert(r >= 0);

		r = GetEngine()->SetEngineProperty(asEP_ALLOW_UNSAFE_REFERENCES, 1); assert(r >= 0);
		r = GetEngine()->SetEngineProperty(asEP_INIT_GLOBAL_VARS_AFTER_BUILD, 1); assert(r >= 0);
		r = GetEngine()->SetEngineProperty(asEP_AUTO_GARBAGE_COLLECT, 1); assert(r >= 0);

		// Very creative interface name
		r = engine->RegisterInterface("IPostal"); assert(r >= 0);

		RegisterStdString(GetEngine());

		RegisterScriptArray(GetEngine(), true);

		RegisterScriptAny(GetEngine());

		// Register the generic handle type, called 'ref' in the script
		RegisterScriptHandle(GetEngine());

		// Register the weak ref template type
		RegisterScriptWeakRef(GetEngine());

		RegisterScriptDictionary(GetEngine());

		// POSTAL 1 stuff
		RegisterCThing(r);
		RegisterCThing3d(r);

		RegisterCCharacter(r);

		RegisterCDoofus(r);

		// This has to be the last
		RegisterGlobal(r);

		// Setup the context manager and register the support for co-routines
		g_ctxMgr = new CContextMgr();
		g_ctxMgr->RegisterCoRoutineSupport(engine);

		// Tell the engine to use our context pool. This will also 
		// allow us to debug internal script calls made by the engine
		r = engine->SetContextCallbacks(RequestContextCallback, ReturnContextCallback, 0); assert(r >= 0);
	}

	CScriptBuilder builder;
	r = builder.StartNewModule(GetEngine(), "PostalModule");
	if (r >= 0)
		r = builder.AddSectionFromFile(g_loaderPath.c_str());
	else
	{
		g_AngelScript.AddLog("Invalid/missing path for loader: '%s'", g_loaderPath.c_str());
		return 0;
	}

	if (r >= 0)
	{
		r = builder.BuildModule();
		if (r >= 0)
		{
			g_AngelScript.AddLog("Successfully built module for '%s'", g_loaderPath.c_str());
			return 1;
		}
		else
		{
			g_AngelScript.AddLog("There were errors in building for '%s'", g_loaderPath.c_str());
			return 0;
		}
	}
	else
	{
		// does it even reach here? Probably not
		g_AngelScript.AddLog("There were errors in building...");
		return 0;
	}

	return 1;
}

void CAngelScriptVM::Recompile()
{
	// TODO: Check if on main menu, it wouldn't be a good idea to do it in-game

	g_AngelScript.ClearLog();
	Setup(true);
}