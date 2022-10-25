#pragma once
#include "BasicTypedefs.h"
extern "C" {
#include "janet.h"
}
#include <string>

class JanetVmService
{
public:
	JanetVmService();
	~JanetVmService();
	void RegisterCFunction(JanetCFunction cFunction, const std::string& name);
	Janet CallJanetFunction(const Janet* argv, i32 argc, const std::string& functionName);
	bool TryLoadJanetFile(const std::string& path);
private:
	JanetTable* m_env;
};

