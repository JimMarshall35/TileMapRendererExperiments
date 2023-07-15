#include "Forth2.h"
#include "ForthStringHelpers.h"
#include <string.h>

/************************************\
 *   Forth - Jim Marshall - 2022    *
 *     jimmarshall35@gmail.com      *
\************************************/

typedef enum {
	Return,
	Add,
	Subtract,
	Divide,
	Multiply,
	Modulo,
	Branch0,
	Branch,
	Dup,
	Swap,
	Rot,
	NumLiteral,
	Emit,
	Fetch,
	Store,
	ByteFetch,
	ByteStore,
	SearchForAndPushExecutionToken,
	Here,
	Allot,
	Colon,
	SemiColon,
	Show,
	ShowWords,
	Immediate,
	Create,
	Equals,
	GreaterThan,
	LessThan,
	And,
	Or,
	Not,
	CellSize,
	CommentStart,
	CommentStop,
	Drop,
	PushReturnStackWord,
	PopReturnStackWord,
	TwoDup,
	TwoSwap,
	TwoRot,
	I,
	J,
	FullStop,
	SearchForAndPushExecutionTokenCompileTime,
	StringLiteral,
	StringLiteralCompileTime,
	EnterWord,
	CallC,
	Key,
	Body,
	SwitchToCompile,
	SwitchToInterpret,
	Does,
	Nop,

	// float instruction set
	// arithmetic
	FAdd,
	FSubtract,
	FDivide,
	FMultiply,
	
	// stack manipulation
	FSwap,
	FDup,
	FRot,
	F2Dup,
	F2Swap,
	F2Rot,

	// comparison
	FEquals,
	FLessthan,
	FGreaterthan,

	// memory
	FFetch,
	FStore,

	// misc
	FLiteral,

	FFullStop,
	FtoI,
	ItoF,

	NumPrimitives // LEAVE AT END
}PrimitiveWordTokenValues;

// forward declarations
static ExecutionToken SearchForToken(ForthVm* vm);
static Bool LoadNextToken(ForthVm* vm); // to be used at compile time only
static int CompileCStringToForthString(ForthVm* vm, const char* string, char delim);




///////////////////////////////////////////////////////////////////////////////////////////////////////////////// Forth String handling functions
//
//	This Forth uses strings of this format:
// 
//	| Cell | byte | byte | .... | .... |
//	|length| char1| char2| char3| ect  |
//

static Bool ParseInlineForthStringToCStringInTokenBuffer(ForthVm* vm, Cell** readPtr) {
	Cell sizeInBytes = *((*readPtr)++);
	char* charCastDest = vm->tokenBuffer;
	char* charCast = (char*)*readPtr;
	for (int i = 0; i < sizeInBytes; i++) {
		charCastDest[i] = charCast[i];
	}
	charCastDest[sizeInBytes] = '\0';
	Cell cellsAdvanceRequired = sizeInBytes % sizeof(Cell) ? (sizeInBytes / sizeof(Cell)) + 1 : sizeInBytes / sizeof(Cell);
	*readPtr += cellsAdvanceRequired;
}



static void PrintInlineForthStringAdvancingReadPointer(const ForthVm* vm, Cell** readPtr, const char* endSequence) {
	int length = *(*readPtr)++;
	int adjustedLength = length % sizeof(Cell) ? (length / sizeof(Cell)) + 1 : length / sizeof(Cell);
	const char* charPtr = *readPtr;
	for (int i = 0; i < length; i++) {
		vm->putchar(charPtr[i]);
	}
	while (*endSequence != '\0') {
		vm->putchar(*endSequence++);
	}
	*readPtr += adjustedLength;
}

static int CompileCStringToForthString(ForthVm* vm, const char* string, char delim) {
	Cell* length = vm->memoryTop++; // save to back-patch with length afterwards;
	const char* readPtr = string;
	char* writePtr = (char*)vm->memoryTop;
	int stringLen = 0;
	while (*readPtr != delim) {
		*writePtr++ = *readPtr++;
		stringLen++;
	}
	Cell cellsRequired = stringLen % sizeof(Cell) ? (stringLen / sizeof(Cell)) + 1 : stringLen / sizeof(Cell);
	*length = stringLen;
	vm->memoryTop += cellsRequired;
	return stringLen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////// Disassembler and stack printing functions

static Bool IsPointingToHeader(const ForthVm* vm, Cell* readPtr) {
	const ForthDictHeader* item = vm->dictionarySearchStart;
	while (item != NULL) {
		if ((const Cell*)item == readPtr) {
			return True;
		}
		item = ((const ForthDictHeader*)item->previous);
	}
	return False;
}

static void PrintDictionaryContents(const ForthVm* vm) {
	const Cell* readPtr = vm->memory;
	while (readPtr < vm->memoryTop) {
		if (IsPointingToHeader(vm, (Cell*)*readPtr)) {
			ForthDictHeader* token = (ForthDictHeader*)(*readPtr);
			ForthPrint(vm, token->name);
			vm->putchar(' ');
			readPtr++;
			switch (token->data[0]) {
			BCase NumLiteral :
			case Branch:
			case Branch0: // intentional fallthrough
				ForthPrintInt(vm, *(readPtr++));
				vm->putchar(' ');
			BCase StringLiteral :
				PrintInlineForthStringAdvancingReadPointer(vm, &readPtr, "\" ");
			BCase SearchForAndPushExecutionToken :
				PrintInlineForthStringAdvancingReadPointer(vm, &readPtr, " ");
			}
		}
		else if (IsPointingToHeader(vm, readPtr)) { // look closer, lenny
			const ForthDictHeader* item = (const ForthDictHeader*)readPtr;
			ForthPrint(vm, " \n");
			ForthPrint(vm, "\n");
			ForthPrint(vm, item->name);
			ForthPrint(vm, "\tImmediate: ");
			ForthPrint(vm, item->isImmediate ? "true" : "false");
			ForthPrint(vm, "\n");
			ForthPrint(vm, "\n");
			ForthPrint(vm, "\t");
			readPtr += (sizeof(ForthDictHeader) / sizeof(Cell));
		}
		else {
			ForthPrintIntHex(vm, *readPtr);
			vm->putchar(' ');
			readPtr++;
		}
	}
	vm->putchar('\n');
	vm->putchar('\n');

	UCell dictionaryBytes = (char*)vm->memoryTop - (char*)vm->memory;
	UCell capacity = vm->maxMemorySize * sizeof(Cell);

	ForthPrint(vm, "dictionary memory usage (bytes): ");
	ForthPrintInt(vm, dictionaryBytes);
	ForthPrint(vm, " / ");
	ForthPrintInt(vm, capacity);
#ifdef CAN_USE_FLOATS
	ForthPrint(vm, ". (");
	float percentage = ((float)dictionaryBytes / (float)capacity) * 100.0f;
	ForthPrintInt(vm, (Cell)percentage);
	ForthPrint(vm, "%)");
#endif
	vm->putchar('\n');
	vm->putchar('\n');
}

static void PrintStack(const ForthVm* vm, Cell* stack, Cell* stackTop, const char* stackName) {
	ForthPrint(vm, stackName);
	Cell* readPtr = stack;
	ForthPrint(vm, "[ ");
	while (readPtr != stackTop) {
		if (readPtr + 1 == stackTop) {
			ForthPrintInt(vm, *(readPtr++));
		}
		else {
			ForthPrintInt(vm, *(readPtr++));
			ForthPrint(vm, ", ");
		}
	}
	ForthPrint(vm, " ]\n");
}

static void PrintFloatStack(const ForthVm* vm) {
	ForthPrint(vm, "float stack:  ");
	FCell* readPtr = vm->floatStack;
	ForthPrint(vm, "[ ");
	while (readPtr != vm->floatStackTop) {
		if (readPtr + 1 == vm->floatStackTop) {
			ForthPrintFloat(vm, *(readPtr++));
		}
		else {
			ForthPrintFloat(vm, *(readPtr++));
			ForthPrint(vm, ", ");
		}
	}
	ForthPrint(vm, " ]\n");
}

static void PrintIntStack(const ForthVm* vm) {
	PrintStack(vm, vm->intStack, vm->intStackTop, "int stack:    ");
}

static void PrintReturnStack(const ForthVm* vm) {
	PrintStack(vm, vm->returnStack, vm->returnStackTop, "return stack: ");
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////// Core internal dictionary manipulation functions

static Bool IsPrimitive(ExecutionToken token) {
	return (token >= 0) && (token < NumPrimitives);
}

static ForthDictHeader* LastWordAdded(ForthVm* vm) {
	return (vm->dictionarySearchStart == NULL) ? vm->memory : vm->dictionarySearchStart;
}

static ExecutionToken SearchForToken(ForthVm* vm) {
	ForthDictHeader* item = vm->dictionarySearchStart;
	while (item != NULL) {
		if (StringCompare(item->name, vm->tokenBuffer)) {
			return item;
		}
		item = item->previous;
	}
	return NULL;
}

static ExecutionToken SearchForTokenString(ForthVm* vm, const char* tokenName) {
	StringCopy(vm->tokenBuffer, tokenName);
	return SearchForToken(vm);
}

static void AddPrimitiveToDict(ForthVm* vm, PrimitiveWordTokenValues primitive, const char* forthName, Bool isImmediate) {
	ForthDictHeader item;
	StringCopy(item.name, forthName);
	item.isImmediate = isImmediate;
	ForthDictHeader* newItem = (ForthDictHeader*)vm->memoryTop;
	*newItem = item;

	// link in new word
	newItem->previous = vm->dictionarySearchStart;
	vm->dictionarySearchStart = newItem;

	// point data to the body and compile a single primitive
	vm->memoryTop += sizeof(ForthDictHeader) / sizeof(Cell); // need to align to make portable
	newItem->data = vm->memoryTop;
	newItem->data[0] = primitive;
	vm->memoryTop++;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////// Inner and outer interpreter

static Bool InnerInterpreter(ForthVm* vm){
	Cell* initialReturnStack = vm->returnStackTop;
	ExecutionToken token, item;
	Cell cell1, cell2, cell3, cell4, cell5, cell6; // top four of the stack commonly used, can't declare locals in case
	FCell fcell1, fcell2, fcell3, fcell4, fcell5, fcell6; // top four of the float stack commonly used, can't declare locals in case

	const char* nextTokenReadPtr = NULL;
	char* dictionaryNameWritePtr = NULL;
	do {
		token = *vm->instructionPointer++;
		switch ((PrimitiveWordTokenValues)token->data[0]) {
		BCase Return:
			vm->instructionPointer = PopReturnStack(vm);
		BCase Add:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			PushIntStack(vm, cell2 + cell1);
		BCase Subtract:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			PushIntStack(vm, cell2 - cell1);
		BCase Divide:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			PushIntStack(vm, cell2 / cell1);
		BCase Multiply:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			PushIntStack(vm, cell2 * cell1);
		BCase Modulo:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			PushIntStack(vm, cell2 % cell1);
		BCase Branch0:
			cell1 = PopIntStack(vm);
			if (cell1 == 0) {
				Cell offset = *vm->instructionPointer;
				vm->instructionPointer += offset;
			}
			else {
				vm->instructionPointer++; // skip over offset
			}
		BCase Branch:
			vm->instructionPointer += *vm->instructionPointer;
		BCase Dup:
			cell1 = PopIntStack(vm);
			PushIntStack(vm, cell1);
			PushIntStack(vm, cell1);
		BCase Swap:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			PushIntStack(vm, cell1);
			PushIntStack(vm, cell2);
		BCase Rot: //( a b c - b c a )
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			cell3 = PopIntStack(vm);
			PushIntStack(vm, cell2);
			PushIntStack(vm, cell1);
			PushIntStack(vm, cell3);
		BCase NumLiteral:
			PushIntStack(vm, *vm->instructionPointer);
			vm->instructionPointer++;
		BCase Emit:
			cell1 = PopIntStack(vm);
			vm->putchar(cell1);
		BCase Fetch:
			cell1 = PopIntStack(vm);
			PushIntStack(vm, *((Cell*)cell1));
		BCase Store:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			*((Cell*)cell1) = cell2;
		BCase ByteFetch:
			cell1 = PopIntStack(vm);
			PushIntStack(vm, ((Cell)*((char*)cell1)));
		BCase ByteStore:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			*((char*)cell1) = (char)cell2;
		BCase SearchForAndPushExecutionToken:
			ParseInlineForthStringToCStringInTokenBuffer(vm, &vm->instructionPointer);
			cell1 = SearchForToken(vm);
			PushIntStack(vm, cell1);
		BCase Here:
			PushIntStack(vm, vm->memoryTop);
		BCase Allot:
			((char*)vm->memoryTop) += PopIntStack(vm);
		BCase Colon:
			if (vm->currentMode & Forth_CompileBit) {
				ForthPrint(vm, "you're already in colon compile mode");
				return True;
			}
			vm->currentMode |= Forth_CompileBit;
			item = (ForthDictHeader*)vm->memoryTop;
			CopyStringUntilSpaceCappingWithNull(item->name, vm->nextTokenStart);
			item->isImmediate = False;
			vm->memoryTop += (sizeof(ForthDictHeader) / sizeof(Cell));
			item->data = vm->memoryTop;
			item->data[0] = EnterWord;
			item->data[1] = &item->data[2];
			vm->memoryTop += 2;
			item->previous = vm->dictionarySearchStart;
			vm->dictionarySearchStart = item;
			LoadNextToken(vm);
		BCase SemiColon:
			if ((vm->currentMode & Forth_CompileBit) == 0) {
				ForthPrint(vm, "you're not in colon compile mode");
			}
			// compile a return token
			*(vm->memoryTop++) = SearchForTokenString(vm, "return");
			// take us out of compile mode
			vm->currentMode &= ~(Forth_CompileBit);
		BCase Show:
			PrintIntStack(vm);
			PrintReturnStack(vm);
			PrintFloatStack(vm);
		BCase ShowWords:
			PrintDictionaryContents(vm);
		BCase Immediate:
			if (vm->currentMode & Forth_CompileBit) {
				ForthPrint(vm, "you can't use the word \"immediate\" when compiling a colon word");
			}
			LastWordAdded(vm)->isImmediate = True;
		BCase Create:
			item = (ForthDictHeader*)vm->memoryTop;
			CopyStringUntilSpaceCappingWithNull(item->name, vm->nextTokenStart);
			item->isImmediate = False;
			vm->memoryTop += (sizeof(ForthDictHeader) / sizeof(Cell));
			
			item->data = vm->memoryTop;
			item->data[0] = EnterWord;
			item->data[1] = &item->data[2];
			// compile the same as any word so far, now compile body of a create-ed word
			/*
			create generated pseudocode:

			push end
			return
			end:

			*/
			item->data[2] = SearchForTokenString(vm, "lit");
			item->data[3] = &item->data[5];
			item->data[4] = SearchForTokenString(vm, "return");
			vm->memoryTop += 5;
			item->previous = vm->dictionarySearchStart;
			vm->dictionarySearchStart = item;
			LoadNextToken(vm);
		BCase Equals:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			PushIntStack(vm, (cell1 == cell2) ? -1 : 0);
		BCase GreaterThan:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			PushIntStack(vm, (cell2 > cell1) ? -1 : 0);
		BCase LessThan:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			PushIntStack(vm, (cell2 < cell1) ? -1 : 0);
		BCase And:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			PushIntStack(vm, ((cell2 != 0) && (cell1 != 0)) ? -1 : 0);
		BCase Or:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			PushIntStack(vm, ((cell2 != 0) || (cell1 != 0)) ? -1 : 0);
		BCase Not:
			cell1 = PopIntStack(vm);
			PushIntStack(vm, (cell1 != 0) ? 0 : -1);
		BCase CellSize:
			PushIntStack(vm, sizeof(Cell));
		BCase CommentStart:
			vm->currentMode |= Forth_CommentFlag;
		BCase CommentStop:
			vm->currentMode &= ~(Forth_CommentFlag);
		BCase Drop:
			PopIntStack(vm);
		BCase PushReturnStackWord:
			cell1 = PopIntStack(vm);
			PushReturnStack(vm, cell1);
		BCase PopReturnStackWord:
			cell1 = PopReturnStack(vm);
			PushIntStack(vm, cell1);
		BCase TwoDup:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			PushIntStack(vm, cell2);
			PushIntStack(vm, cell1);
			PushIntStack(vm, cell2);
			PushIntStack(vm, cell1);
		BCase TwoSwap:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			cell3 = PopIntStack(vm);
			cell4 = PopIntStack(vm);
			PushIntStack(vm, cell2);
			PushIntStack(vm, cell1);
			PushIntStack(vm, cell4);
			PushIntStack(vm, cell3);
		BCase TwoRot:
			cell1 = PopIntStack(vm);
			cell2 = PopIntStack(vm);
			cell3 = PopIntStack(vm);
			cell4 = PopIntStack(vm);
			cell5 = PopIntStack(vm);
			cell6 = PopIntStack(vm);
			PushIntStack(vm, cell4);
			PushIntStack(vm, cell3);
			PushIntStack(vm, cell2);
			PushIntStack(vm, cell1);
			PushIntStack(vm, cell6);
			PushIntStack(vm, cell5);

		BCase I: // these two could be done in forth but we (I) want them to be fast
			cell1 = vm->returnStackTop[-2];
			PushIntStack(vm, cell1);
		BCase J:
			cell1 = vm->returnStackTop[-4];
			PushIntStack(vm, cell1);
		BCase FullStop:
			cell1 = PopIntStack(vm);
			ForthPrintInt(vm, cell1);
		BCase SearchForAndPushExecutionTokenCompileTime:
			*(vm->memoryTop++) = SearchForTokenString(vm, "r'");
			CompileCStringToForthString(vm, vm->nextTokenStart, ' ');
			if (!LoadNextToken(vm)) {
				//return True;
			}
		BCase StringLiteral:
			cell1 = *vm->instructionPointer++; // size of string in bytes
			PushIntStack(vm, cell1);
			PushIntStack(vm, vm->instructionPointer); // ptr to string
			cell2 = cell1 % sizeof(Cell) ? (cell1 / sizeof(Cell)) + 1 : cell1 / sizeof(Cell); // number to advance ip by
			vm->instructionPointer += cell2;
		BCase StringLiteralCompileTime:
			*vm->memoryTop++ = SearchForTokenString(vm, "sr\"");
			cell1 = CompileCStringToForthString(vm, vm->nextTokenStart, '"');
			vm->nextTokenStart += cell1 + 2;
		BCase EnterWord:
			PushReturnStack(vm, vm->instructionPointer);
			vm->instructionPointer = token->data[1];
		BCase CallC:
			((ForthCFunc)token->data[1])(vm);
		BCase Key:
			PushIntStack(vm, vm->getchar());
		BCase Body:
			cell1 = PopIntStack(vm);
			PushIntStack(vm, ((ForthDictHeader*)cell1)->data);
		BCase SwitchToCompile:
			vm->currentMode |= Forth_CompileBit;
		BCase SwitchToInterpret:
			vm->currentMode &= ~Forth_CompileBit;
		BCase Nop: // todo: move below Does when a new token is added to keep in order - can't leave blank if at end of switch

		BCase Does:
			// re-write the return token of the last created word 
			// with a branch pointing to the next part of THIS word.
			// (the last created word SHOULD have been created by does> to work properly as
			// the offset of a create-created word's return value is hard coded here.
			// need to check for proper usage)

			// previous address pushed onto stack by defining word needs to be expanded 
			// to accomodate the fact that we're changing from a return token which is just
			// one cell to a branch token which is two
			vm->dictionarySearchStart->data[3] += sizeof(Cell);

			// swap the return token for a branch token
			vm->dictionarySearchStart->data[4] = SearchForTokenString(vm, "branch");

			{
				// shift memory along by one cell to accomodate extra cell needed for branch token.
				// TODO: research and come up with a more elegant solution. if does> is used as intended should be fine
				Cell offset = vm->memoryTop - &vm->dictionarySearchStart->data[5];
				for (Cell i = 5 + offset; i > 4; i--) {
					vm->dictionarySearchStart->data[i + 1] = vm->dictionarySearchStart->data[i];
				}
			}

			// set offset for branch token
			vm->dictionarySearchStart->data[5] = (vm->instructionPointer - &vm->dictionarySearchStart->data[4]) - 1;

			// increase memory top
			vm->memoryTop++;

			// we don't actually want to run the code after does> when the word with does> in it is executed,
			// we want to link subsequent defined words to it by replacing their return (the whole point of this section of code). 
			// And so we just return now.
			vm->instructionPointer = PopReturnStack(vm);

		BCase FAdd:
			fcell1 = PopFloatStack(vm);
			fcell2 = PopFloatStack(vm);
			PushFloatStack(vm, fcell2 + fcell1);
		BCase FSubtract:
			fcell1 = PopFloatStack(vm);
			fcell2 = PopFloatStack(vm);
			PushFloatStack(vm, fcell2 - fcell1);
		BCase FDivide:
			fcell1 = PopFloatStack(vm);
			fcell2 = PopFloatStack(vm);
			PushFloatStack(vm, fcell2 / fcell1);
		BCase FMultiply:
			fcell1 = PopFloatStack(vm);
			fcell2 = PopFloatStack(vm);
			PushFloatStack(vm, fcell2 * fcell1);
		BCase FSwap:
			fcell1 = PopFloatStack(vm);
			fcell2 = PopFloatStack(vm);
			PushFloatStack(vm, fcell1);
			PushFloatStack(vm, fcell2);
		BCase FDup:
			fcell1 = PopFloatStack(vm);
			PushFloatStack(vm, fcell1);
			PushFloatStack(vm, fcell1);
		BCase FRot:
			fcell1 = PopFloatStack(vm);
			fcell2 = PopFloatStack(vm);
			fcell3 = PopFloatStack(vm);
			PushFloatStack(vm, fcell2);
			PushFloatStack(vm, fcell1);
			PushFloatStack(vm, fcell3);
		BCase F2Dup:
			fcell1 = PopFloatStack(vm);
			fcell2 = PopFloatStack(vm);
			PushFloatStack(vm, fcell2);
			PushFloatStack(vm, fcell1);
			PushFloatStack(vm, fcell2);
			PushFloatStack(vm, fcell1);
		BCase F2Swap:
			fcell1 = PopFloatStack(vm);
			fcell2 = PopFloatStack(vm);
			fcell3 = PopFloatStack(vm);
			fcell4 = PopFloatStack(vm);
			PushFloatStack(vm, fcell2);
			PushFloatStack(vm, fcell1);
			PushFloatStack(vm, fcell4);
			PushFloatStack(vm, fcell3);
		BCase F2Rot:
			fcell1 = PopFloatStack(vm);
			fcell2 = PopFloatStack(vm);
			fcell3 = PopFloatStack(vm);
			fcell4 = PopFloatStack(vm);
			fcell5 = PopFloatStack(vm);
			fcell6 = PopFloatStack(vm);
			PushFloatStack(vm, fcell4);
			PushFloatStack(vm, fcell3);
			PushFloatStack(vm, fcell2);
			PushFloatStack(vm, fcell1);
			PushFloatStack(vm, fcell6);
			PushFloatStack(vm, fcell5);
		BCase FEquals:
			fcell1 = PopFloatStack(vm);
			fcell2 = PopFloatStack(vm);
			PushFloatStack(vm, (fcell1 == fcell2) ? -1 : 0);
		BCase FLessthan:
			fcell1 = PopFloatStack(vm);
			fcell2 = PopFloatStack(vm);
			PushFloatStack(vm, (fcell2 < fcell1) ? -1 : 0);
		BCase FGreaterthan:
			fcell1 = PopFloatStack(vm);
			fcell2 = PopFloatStack(vm);
			PushFloatStack(vm, (fcell2 > fcell1) ? -1 : 0);
		BCase FFetch:
			cell1 = PopIntStack(vm);
			PushFloatStack(vm, *((FCell*)cell1));
		BCase FStore :
			cell1 = PopIntStack(vm);
			fcell1 = PopFloatStack(vm);
			*((FCell*)cell1) = fcell1;
		BCase FLiteral:
			PushFloatStack(vm, *vm->instructionPointer);
			vm->instructionPointer++;
		BCase FFullStop:
			fcell1 = PopIntStack(vm);
			ForthPrintFloat(vm, fcell1);
		BCase FtoI:
			PushIntStack(vm, (Cell)PopFloatStack(vm))
		BCase ItoF:
			PushFloatStack(vm, (FCell)PopIntStack(vm));
		}
	} while (vm->returnStackTop != initialReturnStack);
	return False;
}

static Bool LoadNextToken(ForthVm* vm) {
	int len = CopyNextToken(vm->nextTokenStart, vm->tokenBuffer);
	vm->nextTokenStart += len;
	while (*vm->nextTokenStart == ' ')vm->nextTokenStart++;
	if (len == 0) {
		return False; // stream has finished
	}
	else {
		return True; // continue reading stream
	}
}

static void HandlePossibleLiteral(ForthVm* vm)
{
	
	if (StringContains(vm->tokenBuffer, '.') == True) {
		FCell converted = ForthStrtod(vm->tokenBuffer);
		if (vm->currentMode & Forth_CompileBit) {

			// compile number literal
			ExecutionToken lit = SearchForTokenString(vm, "flit");
			*(vm->memoryTop++) = lit;
			*(vm->memoryTop++) = converted;
		}
		else {
			PushFloatStack(vm, converted);
		}
	}
	else {
		Cell converted = ForthAtoi(vm->tokenBuffer);
		if (vm->currentMode & Forth_CompileBit) {

			// compile number literal
			ExecutionToken lit = SearchForTokenString(vm, "lit");
			*(vm->memoryTop++) = lit;
			*(vm->memoryTop++) = converted;
		}
		else {
			PushIntStack(vm, converted);
		}
	}
	
}

static void OuterInterpreter(ForthVm* vm, const char* input) {
	vm->nextTokenStart = input;
	
	if (!LoadNextToken(vm)) {
		return;
	}
	while (*vm->tokenBuffer != "\0") {
		ExecutionToken foundToken = SearchForToken(vm);
		vm->instructionPointer = &foundToken;
		if (foundToken != NULL) {
			if (vm->currentMode & Forth_CompileBit) {
				if ((vm->currentMode & Forth_CommentFlag) == 0) {
					if (foundToken->isImmediate) {
						InnerInterpreter(vm);
					}
					else {
						*(vm->memoryTop++) = foundToken;
					}
				}
				else if (foundToken->data[0] == CommentStop) {
					InnerInterpreter(vm);
				}
			}
			else {
				InnerInterpreter(vm);
			}
		}
		else if((vm->currentMode & Forth_CommentFlag) == 0){
			HandlePossibleLiteral(vm);
		}
		if (!LoadNextToken(vm)) {
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////// Core forth words

static const char* coreWords =
// helpers
": allotCell cell * allot ; "

": , ( valueToCompile -- ) here ! 1 allotCell ; "

": cells cell / ; "

": branchOffsetInCells ( addressOfBranch -- offsetToTheAddressInCellsFromCurrentDictPointer ) "
	"here swap - ( calculate offset in bytes ) "
	"cells ( convert to offset in cells ) "
"; "

": backPatch ( addressOfBranch -- ) "
	"branchOffsetInCells "
	"swap ! ( store the offset at the address to be back-patched ) "
"; "

// immediate words - these compile themselves down into control flow structures made up of branch and branch0 only
// and these automate the setting of branch offsets by running at compile time as denoted by "immediate"
": if "
	"' branch0 , ( compile a branch0 token ) "
	"here ( 'here' now points to where the compiled ifs branch offset will go, push it to be back-patched by then or else ) "
	"1 allotCell ( advance top of memory to compile next part of the program ) "
"; immediate "

": then "
	"dup ( duplicate the address that will be set with the branch offset ) "
	"backPatch "
"; immediate "

": else "
	"' branch , ( compile branch token ) "
	"here ( the top of memory now points to the cell where the branch tokens offset is, push it so it can be back - patched by then ) "
	"1 allotCell ( move to the cell after, we will calculate the offset to this cell to back patch the if ) "
	"swap ( swap so that the if tokens branch offset address is on top ) "
	"dup ( two copies of the if's branch offset address are on top, and under that is this elses branch offset address ) "
	"backPatch "
"; immediate "

// loops 
": begin "
	"here "
"; immediate "

": until "
	"' branch0 , ( compile a branch0 token ) "
	"branchOffsetInCells "
	"-1 * "
	"here ! "
	"1 allotCell ( advance top of memory to compile next part of the program ) "
"; immediate "
/*
*  compiler generated do / loop pseudo code
* https://sincereflattery.blog/2020/07/08/i-do-forth/
* *******************************************

   jump to test
start:
	save indexes to return stack     // do is up to and including this line

	loop content

	retrieve indexes from return stack // loop starts here
	add one to index
test:
	if index has not reached the end, jump to start
exit:
	clean up of indexes //ends here
	rest of code

*/
": do ( limit i -- ) "
	"' branch , "
	"here ( label of initial jump ) "
	"1 allotCell "
	"here ( start label in pseudo code above ) "
	"swap "
	"' R , ( compile code to push i onto return stack ) "
	"' R , ( compile code to push limit onto return stack ) "
"; immediate "

": loop "
	"( compile code to pop i and limit from return stack ) "
	"' R> , "
	"' R> , "

	"( compile code to increment i ) "
	"' lit , "
	"1 , "
	"' + , "

	"( we are now at the test label ) "
	"dup "
	"backPatch "

	"( compile code to compare i and limit and branch if not equal ) "
	"' 2dup , "
	"' = , "
	"' branch0 , "
	"here "
	"- cell "
	"/ , "

	"( compile code to clean up i and limit from int stack now that the loop has ended ) "
	"' drop , "
	"' drop , "
"; immediate "

": +loop ( amountToIncrementIBy -- ) "
	"( compile code to pop i and limit from return stack ) "
	"' R> , "
	"' R> , "

	"( compile code to rotate increment amount from bottom of stack ) "
	"' rot , "
	"' + , "

	"( we are now at the test label ) "
	"dup "
	"backPatch "

	"( compile code to compare i and limit and branch if not equal ) "
	"' 2dup , "
	"' = , "
	"' branch0 , "
	"here "
	"- cell "
	"/ , "

	"( compile code to clean up i and limit from int stack now that the loop has ended ) "
	"' drop , "
	"' drop , "
"; immediate "

// other misc words

": cr 10 emit ; "

": print ( length address -- ) "
	"swap "
	"0 do "
		"dup i + c@ emit "
	"loop drop "
"; "

": var ( [consumes next input token] initialVal -- ) create , ; "

": const ( [consumes next input token] initialVal -- ) var does> @ ; "
": array ( [consumes next token] arraySize -- ) create 0 do 0 , loop ; "
;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////// Public API

ExecutionToken Forth_RegisterCFunc(ForthVm* vm, ForthCFunc function, const char* name, Bool isImmediate) {
	ForthDictHeader item;
	StringCopy(item.name, name);
	item.isImmediate = isImmediate;
	ForthDictHeader* newItem = (ForthDictHeader*)vm->memoryTop;
	ExecutionToken newToken = newItem;
	*newItem = item;

	// link in new word
	newItem->previous = vm->dictionarySearchStart;
	vm->dictionarySearchStart = newItem;

	// point data to the body and compile a CallC primitive and C function ptr address
	vm->memoryTop += sizeof(ForthDictHeader) / sizeof(Cell); // need to align to make portable
	newItem->data = vm->memoryTop;
	newItem->data[0] = CallC;
	newItem->data[1] = function;
	vm->memoryTop += 2;

	return newToken;
}

ExecutionToken Forth_SearchForExecutionToken(ForthVm* vm, const char* tokenName)
{
	return SearchForTokenString(vm, tokenName);
}

Bool Forth_DoExecutionToken(ForthVm* vm, ExecutionToken xt)
{
	vm->instructionPointer = &xt;
	return InnerInterpreter(vm);
}


ForthVm Forth_Initialise(
	Cell* memoryForCompiledWordsAndVariables,
	UCell memorySize,
	Cell* intStack,
	UCell intStackSize,
	Cell* returnStack,
	UCell returnStackSize,
	Cell* floatStack,
	UCell floatStackSize,
	ForthPutChar putc,
	ForthGetChar getc) {

	ForthVm vm;
	
	vm.dictionarySearchStart = NULL;

	vm.currentMode = 0;

	vm.memory = memoryForCompiledWordsAndVariables;
	vm.memoryTop = vm.memory;
	vm.maxMemorySize = memorySize;

	vm.intStack = intStack;
	vm.intStackTop = vm.intStack;
	vm.maxIntStackSize = intStackSize;

	vm.returnStack = returnStack;
	vm.returnStackTop = vm.returnStack;
	vm.maxReturnStackSize = returnStackSize;

	vm.floatStack = floatStack;
	vm.floatStackTop = vm.floatStack;
	vm.maxFloatStackSize = floatStackSize;

	vm.putchar = putc;
	vm.getchar = getc;

	vm.instructionPointer = vm.memory;

	AddPrimitiveToDict(&vm, Return,                                    "return",    False);
	AddPrimitiveToDict(&vm, Add,                                       "+",         False);
	AddPrimitiveToDict(&vm, Subtract,                                  "-",         False);
	AddPrimitiveToDict(&vm, Divide,                                    "/",         False);
	AddPrimitiveToDict(&vm, Multiply,                                  "*",         False);
	AddPrimitiveToDict(&vm, Modulo,                                    "%",         False);
	AddPrimitiveToDict(&vm, Branch0,                                   "branch0",   False);
	AddPrimitiveToDict(&vm, Branch,                                    "branch",    False);
	AddPrimitiveToDict(&vm, Dup,                                       "dup",       False);
	AddPrimitiveToDict(&vm, Swap,                                      "swap",      False);
	AddPrimitiveToDict(&vm, Rot,                                       "rot",       False);
	AddPrimitiveToDict(&vm, NumLiteral,                                "lit",       False);
	AddPrimitiveToDict(&vm, Emit,                                      "emit",      False);
	AddPrimitiveToDict(&vm, Fetch,                                     "@",         False);
	AddPrimitiveToDict(&vm, Store,                                     "!",         False);
	AddPrimitiveToDict(&vm, ByteFetch,                                 "c@",        False);
	AddPrimitiveToDict(&vm, ByteStore,                                 "c!",        False);
	AddPrimitiveToDict(&vm, SearchForAndPushExecutionToken,            "r'",        False);
	AddPrimitiveToDict(&vm, Here,                                      "here",      False);
	AddPrimitiveToDict(&vm, Allot,                                     "allot",     False);
	AddPrimitiveToDict(&vm, Colon,                                     ":",         False);
	AddPrimitiveToDict(&vm, SemiColon,                                 ";",         True);
	AddPrimitiveToDict(&vm, Show,                                      "show",      False);
	AddPrimitiveToDict(&vm, ShowWords,                                 "showWords", False);
	AddPrimitiveToDict(&vm, Immediate,                                 "immediate", False);
	AddPrimitiveToDict(&vm, Create,                                    "create",    False);
	AddPrimitiveToDict(&vm, Equals,                                    "=",         False);
	AddPrimitiveToDict(&vm, GreaterThan,                               ">",         False);
	AddPrimitiveToDict(&vm, LessThan,                                  "<",         False);
	AddPrimitiveToDict(&vm, And,                                       "and",       False);
	AddPrimitiveToDict(&vm, Or,                                        "or",        False);
	AddPrimitiveToDict(&vm, Not,                                       "not",       False);
	AddPrimitiveToDict(&vm, CellSize,                                  "cell",      False);
	AddPrimitiveToDict(&vm, CommentStart,                              "(",         True);
	AddPrimitiveToDict(&vm, CommentStop,                               ")",         True);
	AddPrimitiveToDict(&vm, Drop,                                      "drop",      False);
	AddPrimitiveToDict(&vm, PushReturnStackWord,                       "R",         False);
	AddPrimitiveToDict(&vm, PopReturnStackWord,                        "R>",        False);
	AddPrimitiveToDict(&vm, TwoDup,                                    "2dup",      False);
	AddPrimitiveToDict(&vm, TwoSwap,                                   "2swap",     False);
	AddPrimitiveToDict(&vm, TwoRot,                                    "2rot",      False);
	AddPrimitiveToDict(&vm, I,                                         "i",         False);
	AddPrimitiveToDict(&vm, J,                                         "j",         False);
	AddPrimitiveToDict(&vm, FullStop,                                  ".",         False);
	AddPrimitiveToDict(&vm, SearchForAndPushExecutionTokenCompileTime, "'",         True);
	AddPrimitiveToDict(&vm, StringLiteral,                             "sr\"",      False);
	AddPrimitiveToDict(&vm, StringLiteralCompileTime,                  "s\"",       True);
	AddPrimitiveToDict(&vm, EnterWord,                                 "enter",     False);
	AddPrimitiveToDict(&vm, CallC,                                     "call",      False); // it's not really necessary to add these last two as primitives here but I have done anyway
	AddPrimitiveToDict(&vm, Key,                                       "key",       False);
	AddPrimitiveToDict(&vm, Body,                                      ">body",     False); // todo: check if used
	AddPrimitiveToDict(&vm, SwitchToCompile,                           "]",         False);
	AddPrimitiveToDict(&vm, SwitchToInterpret,                         "[",         True);
	AddPrimitiveToDict(&vm, Does,                                      "does>",     False);
	AddPrimitiveToDict(&vm, Nop,                                       "nop",       False);
	// floating point
	AddPrimitiveToDict(&vm, FAdd,         "f+",False);
	AddPrimitiveToDict(&vm, FSubtract,    "f-", False);
	AddPrimitiveToDict(&vm, FDivide,      "f/", False);
	AddPrimitiveToDict(&vm, FMultiply,    "f*", False);

	AddPrimitiveToDict(&vm, FSwap,        "fswap", False);
	AddPrimitiveToDict(&vm, FDup,         "fdup", False);
	AddPrimitiveToDict(&vm, FRot,         "frot", False);
	AddPrimitiveToDict(&vm, F2Dup,        "f2dup", False);
	AddPrimitiveToDict(&vm, F2Swap,       "fswap", False);
	AddPrimitiveToDict(&vm, F2Rot,        "f2rot", False);

	AddPrimitiveToDict(&vm, FEquals,      "f=", False);
	AddPrimitiveToDict(&vm, FLessthan,    "f<", False);
	AddPrimitiveToDict(&vm, FGreaterthan, "f>", False);

	AddPrimitiveToDict(&vm, FFetch,       "f@", False);
	AddPrimitiveToDict(&vm, FStore,       "f!", False);
	AddPrimitiveToDict(&vm, FLiteral,     "flit", False);
	AddPrimitiveToDict(&vm, FFullStop,    "f.", False);
	AddPrimitiveToDict(&vm, FtoI,         "f2i", False);
	AddPrimitiveToDict(&vm, ItoF,         "i2f", False);


	// load core vocabulary of words that are not primitive, ie are defined in forth
	OuterInterpreter(&vm, coreWords);

	return vm;
}
void Forth_Teardown()
{
}

Bool Forth_DoString(ForthVm* vm, const char* inputString)
{
	OuterInterpreter(vm, inputString);
	return True;
}
