#include "as_main.h"
#include <cstdarg>

bool replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

static void GetVarArgsFormatting(asIScriptGeneric* gen, std::string& format)
{
	void* ref = gen->GetArgAddress(0);
	int typeId = gen->GetArgTypeId(0);

	format = *static_cast<std::string*>(ref);

	for (int i = 1; i < 16; i++)
	{
		ref = gen->GetArgAddress(i);
		typeId = gen->GetArgTypeId(i);

		switch (typeId)
		{
		case 67108876: //string?
		{
			if (ref)
			{
				std::string local = *static_cast<std::string*>(ref);

				if (local.length() <= 0)
					local = "(null)";

				replace(format, "%s", local);
			}
			else
			{
				std::string local = "(null)";
				replace(format, "%s", local);
			}
			break;
		}
		case 2:
		{
			char local = *static_cast<char*>(ref);
			const char* l = &local;
			replace(format, "%d", l);
			break;
		}
		case 3:
		{
			short local = *static_cast<short*>(ref);
			const char* l = (char*)local;
			replace(format, "%d", l);
			break;
		}
		case 4:
		{
			int local = *static_cast<int*>(ref);
			char buf[64];
			_snprintf(buf, 64, "%d", local);
			buf[64-1] = '/0';
			replace(format, "%d", buf);
			break;
		}
		case 5:
		{
			long long local = *static_cast<long long*>(ref);
			char buf[64];
			_snprintf(buf, 64, "%d", (int)local);
			buf[64 - 1] = '/0';
			replace(format, "%d", buf);
			break;
		}
		case 6:
		{
			unsigned char local = *static_cast<unsigned char*>(ref);
			char buf[64];
			_snprintf(buf, 64, "%d", local);
			buf[64 - 1] = '/0';
			replace(format, "%d", buf);
			break;
		}
		case 7:
		{
			unsigned short local = *static_cast<unsigned short*>(ref);
			char buf[64];
			_snprintf(buf, 64, "%d", local);
			buf[64 - 1] = '/0';
			replace(format, "%d", buf);
			break;
		}
		case 8:
		{
			unsigned int local = *static_cast<unsigned int*>(ref);
			char buf[64];
			_snprintf(buf, 64, "%d", local);
			buf[64 - 1] = '/0';
			replace(format, "%d", buf);
			break;
		}
		case 9:
		{
			unsigned long long local = *static_cast<unsigned long long*>(ref);
			char buf[64];
			_snprintf(buf, 64, "%d", (unsigned int)local);
			buf[64 - 1] = '/0';
			replace(format, "%d", buf);
			break;
		}
		case 10:
		{
			float local = *static_cast<float*>(ref);
			char buf[64];
			_snprintf(buf, 64, "%f", local);
			buf[64 - 1] = '/0';
			replace(format, "%f", (const char*)buf);
			break;
		}
		case 11:
		{
			double local = *static_cast<double*>(ref);
			char buf[64];
			_snprintf(buf, 64, "%f", local);
			buf[64 - 1] = '/0';
			replace(format, "%f", (const char*)buf);
			break;
		}
		}
	}
}

static void asLog(const std::string& str, ...)
{
	const char* MSG = str.c_str();

	va_list		argptr;
	char		string[1024];
	va_start(argptr, MSG);
	_vsnprintf(string, sizeof(string), MSG, argptr);
	va_end(argptr);

	g_AngelScript.AddLog(LOG_SCRIPT, string);
}

static void asVarArgs(asIScriptGeneric* gen)
{
	std::string format;
	GetVarArgsFormatting(gen, format);

	asLog(format);
}

void CAngelScriptVM::RegisterGlobal(int& r)
{
	AS_REGISTER_GLOBAL_G("void Log(string&in msg, ?&in var = 0, ?&in var2 = 0, ?&in var3 = 0, ?&in var4 = 0, ?&in var5 = 0, ?&in var6 = 0, ?&in var7 = 0, ?&in var8 = 0, ?&in var9 = 0, ?&in var10 = 0, ?&in var11 = 0, ?&in var12 = 0, ?&in var13 = 0, ?&in var14 = 0, ?&in var15 = 0)", asVarArgs);
}