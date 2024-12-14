#ifndef POSTAL_ANGELSCRIPT_H
#define POSTAL_ANGELSCRIPT_H
#pragma once

#include "angelscript.h"
#include <string>
#include <vector>

class CAngelScriptVM
{
public:
	CAngelScriptVM();
	int Setup(bool bRecompile);

	void AddLog(const char* str, ...);
	//{
		//log.push_back(str);
	//}

public:
	asIScriptEngine* GetEngine() { return engine; }
	asIScriptModule* GetModule() { return mod; }

	std::vector<std::string> GetLog() { return log; }
	void ClearLog() { log.clear(); }

	void Recompile();

private:
	asIScriptEngine* engine;
	asIScriptModule* mod;

	// oh no
	//std::string log;

	std::vector<std::string> log;
};

// Accessor
extern CAngelScriptVM g_AngelScript;

#endif // POSTAL_ANGELSCRIPT_H