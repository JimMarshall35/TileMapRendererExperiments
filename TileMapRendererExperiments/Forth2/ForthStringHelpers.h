#ifndef STRINGUTILS
#define STRINGUTILS
#ifdef __cplusplus
extern "C" {
#endif
#include "ForthCommonTypedefs.h"

Cell StringLength(const char* string);
Bool StringCompare(const char* string1, const char* string2);
void StringCopy(char* destination, const char* source);
int CopyNextToken(const char* inputString, char* tokenOutput);
void CopyStringUntilSpaceCappingWithNull(char* dest, const char* src);
Cell ForthAtoi(const char* string);
void ForthPrint(const ForthVm* vm, const char* string);
void ForthPrintInt(const ForthVm* vm, Cell cell);
void ForthPrintIntHex(const ForthVm* vm, Cell cell);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif
#endif