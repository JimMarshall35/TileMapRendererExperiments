#include "ForthStringHelpers.h"
#include <stdlib.h>     /* strtod */
#include <stdio.h> /* printf */

Bool StringContains(const char* string, char character)
{
	int i = 0;
	while (string[i] != '\0'){
		if (string[i++] == character) {
			return True;
		}
	}
	return False;
}

Cell StringLength(const char* string)
{
	int len = 0;
	while (*(string++) != '\0') {
		len++;
	}
	return len;
}

Bool StringCompare(const char* string1, const char* string2) {
	const char* readPtr1 = string1;
	const char* readPtr2 = string2;

	while (*readPtr1 != '\0') {
		if (*readPtr1 != *readPtr2) {
			return False;
		}
		readPtr1++;
		readPtr2++;
	}
	if (*readPtr2 != '\0') {
		return False;
	}
	return True;
}

void StringCopy(char* destination, const char* source) {
	char* destinationWritePtr = destination;
	const char* srcReadPtr = source;
	while (*srcReadPtr != '\0') {
		*destinationWritePtr = *srcReadPtr;
		srcReadPtr++;
		destinationWritePtr++;
	}
	*destinationWritePtr = '\0';
}

int CopyNextToken(const char* inputString, char* tokenOutput)
{
	Bool shouldContinue = True;
	const char* thisChar = inputString;
	char* writePtr = tokenOutput;
	int length = 0;
	while (shouldContinue) {
		char thisCharVal = *(thisChar++);
		if (thisCharVal == ' ') { // finished reading token
			*writePtr = '\0';
			writePtr = tokenOutput;
			shouldContinue = False;
		}
		else if (thisCharVal == '\0') {  // finished reading string
			*writePtr = '\0';
			shouldContinue = False;
		}
		else { // read character of string
			*(writePtr++) = thisCharVal;
			length++;
		}

	}
	return length;
}

void CopyStringUntilSpaceCappingWithNull(char* dest, const char* src) {
	do {
		*(dest++) = *(src++);
	} while (*src != ' ');
	*dest = '\0';
}

Cell ForthAtoi(const char* string)
{
	Cell signMultiplier = 1;
	Cell endPoint = 0;
	if (string[0] == '-') {
		signMultiplier = -1;
		endPoint = 1;
	}

	Cell length = StringLength(string);
	Cell rVal = 0;
	Cell unitsTensHundredsEct = 1;
	for (Cell i= length-1; i >= endPoint; --i) {
		Cell value = string[i] - '0';
		rVal += value * unitsTensHundredsEct;
		unitsTensHundredsEct *= 10;
	}

	return rVal * signMultiplier;
}

void ForthPrint(const ForthVm* vm, const char* string)
{
	char* thisChar = string;
	while (*thisChar != '\0') {
		vm->putchar(*thisChar++);
	}
}

void PrintNibbles(const ForthVm* vm, int numNibblesToPrint) {
	for (int i = 0; i < numNibblesToPrint; i++) {
		vm->putchar('0');
	}
}

/* enforceCellSize = do we print pad extra 0s to hex numbers to reflect cell size */
void ForthPrintIntInternal(const ForthVm* vm, Cell val, int base, Bool enforceCellSize)
{
	static char buf[32] = { 0 };
	int i = 30;
	if (val == 0) {
		if (enforceCellSize) {
			switch (base)
			{
				BCase 10: // not implemented
				BCase 16 :
					PrintNibbles(vm, sizeof(Cell) * 2);
			}
		}
		else {
			vm->putchar('0');
		}
		return;
	}
	Cell absVal = (val < 0) ? -val : val;
	UCell numDigits = 0;
	for (; absVal && i; --i, absVal /= base) {
		buf[i] = "0123456789abcdef"[absVal % base];
		numDigits++;
	}
	if (val < 0) {
		buf[i--] = '-';
	}
	char* string = &buf[i + 1];
	if (enforceCellSize) {
		switch (base)
		{
		BCase 10: // not implemented
		BCase 16:
			PrintNibbles(vm, sizeof(Cell) * 2 - numDigits);
		}
	}
	ForthPrint(vm, string);
}

void ForthPrintInt(const ForthVm* vm, Cell val)
{
	ForthPrintIntInternal(vm, val, 10, False);
}

void ForthPrintIntHex(const ForthVm* vm, Cell val)
{
	ForthPrint(vm, "0x");
	ForthPrintIntInternal(vm, val, 16, True);
}

void ForthPrintFloat(const ForthVm* vm, FCell val)
{
	printf("%lf", val);
	// need to replace with own implementation before merging back into main forth repo
}

FCell ForthStrtod(const char* string)
{
	char* ptr;
	FCell cell = strtod(string, &ptr);
	return cell;
	// need to replace with own implementation before merging back into main forth repo
}

