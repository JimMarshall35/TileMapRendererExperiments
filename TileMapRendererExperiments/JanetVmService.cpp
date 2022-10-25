#include "JanetVmService.h"
#include <iostream>
#include <string>
#include <fstream>



JanetVmService::JanetVmService() {
	janet_init();
	m_env = janet_core_env(NULL);
	
}

JanetVmService::~JanetVmService()
{
	janet_deinit();
}

void JanetVmService::RegisterCFunction(JanetCFunction cFunction, const std::string& name)
{
	janet_def_sm(m_env, name.c_str(), janet_wrap_cfunction(cFunction), "", "", 0);
}

Janet JanetVmService::CallJanetFunction(const Janet* argv, i32 argc, const std::string& functionName)
{
	Janet returnval;
	JanetBindingType result = janet_resolve(m_env, janet_csymbol(functionName.c_str()), &returnval);
	//assert(result == JANET_BINDING_DEF);
	if (result != JANET_BINDING_DEF) {
		std::cerr << "result != JANET_BINDING_DEF\n";
		return janet_wrap_nil();
	}
	/* Use whatever arguments you want. Below is my-fn is called with 0 arguments. */
	janet_pcall(janet_unwrap_function(returnval), argc,argv,&returnval,NULL);
	return returnval;

}

bool JanetVmService::TryLoadJanetFile(const std::string& path)
{
	std::ifstream fs;
	try {
		fs = std::ifstream(path);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << "\n";
		return false;
	}
	std::string file = "";
	std::string line = "";
	while (std::getline(fs, line)) {
		file += line;
	}
	janet_dostring(m_env, file.c_str(), "", NULL);
}
