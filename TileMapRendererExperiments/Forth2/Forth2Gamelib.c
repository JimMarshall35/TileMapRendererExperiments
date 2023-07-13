//#include "ForthCommonTypedefs.h"
//
//﻿#include <stdio.h>
//
//
///*
//Applies this transformation to the contents of file fp:
//removes tabs, changes newlines into spaces, limits spaces (outside of string literals and comments) to only one concecutive space.
//writes result into buf
//*/
//void PreProcessForthSourceFileContentsIntoBuffer(FILE* fp, char* buf) {
//    char* writePtr = buf;
//    Bool inStringOrComment = False;
//    int consecutiveSpaceCount = 0;
//    while (!feof(fp)) {
//        char fileChar = (char)fgetc(fp);
//        if (!inStringOrComment) {
//            switch (fileChar) {
//                BCase '\t':
//                // ignore tabs
//            case '\n':
//            case ' ': // intentional fallthrough
//                // only ever one space in a row
//                if (consecutiveSpaceCount++ == 0) {
//                    *writePtr++ = ' ';
//                }
//                BCase '"' :
//                    if (*(writePtr - 1) == 's') {
//                        inStringOrComment = True;
//                    }
//                *writePtr++ = fileChar;
//                consecutiveSpaceCount = 0;
//                BCase '(':
//                if (*(writePtr - 1) == ' ') {
//                    inStringOrComment = True;
//                }
//                *writePtr++ = fileChar;
//                consecutiveSpaceCount = 0;
//            BDefault:
//                *writePtr++ = fileChar;
//                consecutiveSpaceCount = 0;
//            }
//        }
//        else {
//            if (fileChar == '"' || fileChar == ')') { // TODO: make more robust to allow ) in string literals and " in comments
//                inStringOrComment = False;
//            }
//            *writePtr++ = fileChar;
//        }
//    }
//    writePtr[-1] = '\0';
//}
