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

//extern CAngelScriptVM* g_AngelScript;
CAngelScriptVM g_AngelScript;

CAngelScriptVM::CAngelScriptVM()
{
	engine = nullptr;
	mod = nullptr;
	log.clear();
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

static std::string g_loaderPath = GetCurrentDir() + std::string("/scripts/angelscript.txt");

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

		RegisterStdString(GetEngine());

		RegisterScriptArray(GetEngine(), true);

		RegisterScriptAny(GetEngine());

		// Register the generic handle type, called 'ref' in the script
		RegisterScriptHandle(GetEngine());

		// Register the weak ref template type
		RegisterScriptWeakRef(GetEngine());

		RegisterScriptDictionary(GetEngine());
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