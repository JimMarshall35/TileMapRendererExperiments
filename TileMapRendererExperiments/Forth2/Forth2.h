
#ifndef FORTH2
#define FORTH2
#ifdef __cplusplus
extern "C" {
#endif


#include "ForthCommonTypedefs.h"
	/************************************\
	 *   Forth - Jim Marshall - 2022    *
	 *     jimmarshall35@gmail.com      *
	\************************************/
	/*
		Core library - Forth
		Game scripting language edition.
		Planned changes:
			(definite) further c/c++ support. excecute a function through a c++ ptr to forth threaded code rather than name
			(definite) add a float stack
			(possible) add a vec2 stack
			(probable) error messages
			
	*/

	typedef Bool(*ForthCFunc)(ForthVm* vm);

	// only obtain a forth Vm like this
	ForthVm Forth_Initialise(
		Cell* memoryForCompiledWordsAndVariables,
		UCell memorySize,
		Cell* intStack,
		UCell intStackSize,
		Cell* returnStack,
		UCell returnStackSize,
		ForthPutChar putc,
		ForthGetChar getc);

	Bool Forth_DoString(ForthVm* vm, const char* inputString);
	void Forth_RegisterCFunc(ForthVm* vm, ForthCFunc function, const char* name, Bool isImmediate);



#ifdef __cplusplus
} // closing brace for extern "C"
#endif
#endif