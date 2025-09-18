#include "BinarySerializer.h"
#include "Atlas.h"
#include "DrawContext.h"
#include "ImageFileRegstry.h"
#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

struct Args
{
	char* xmlPath;
	char* outPath;
}args;

const char* defaultOutPath = "out.atlas";

void PrintHelpMsg()
{
	printf(
		"Atlas Tool\n"
		"Function:\n"
		"          Convert an xml description of an atlas into a binary atlas file\n"
		"          The binary file contains the pre-made atlas image for fast loading\n"
		"Args:\n"
		"          AtlasTool[inXMLPath][Args]\n"
		"          -o \"outputBinPath.atlas\"\n"
	);
}

int ParseArgs(int argc, char** argv)
{
#ifdef WIN32
	memset(&args, 0, sizeof(struct Args));
	if (argc < 2)
	{
		printf("At least one arg expected\n");
		return 1;
	}
	args.xmlPath = argv[1];
	for (int i = 2; i < argc; i++)
	{
		if (strcmp(argv[i], "-o") == 0)
		{
			args.outPath = argv[i + 1];
		}
		else if (strcmp(argv[i], "-h") == 0)
		{
			PrintHelpMsg();
		}
	}
	if (!args.outPath)
	{
		args.outPath = defaultOutPath;
	}
	return 0;
#else
#endif
	
}

static hTexture UploadTextureMock(void* src, int channels, int pxWidth, int pxHeight)
{
	return 0;
}

int main(int argc, char** argv)
{
	int r = ParseArgs(argc, argv);
	printf("input file: %s\n", args.xmlPath);
	printf("output file: %s\n", args.outPath);
	fflush(stdout);
	if (r)
	{
		return r;
	}

	IR_InitImageRegistry(NULL);
	At_Init();
	xmlDoc* pXMLDoc = xmlReadFile(args.xmlPath, NULL, 0);
	xmlNode* root = xmlDocGetRootElement(pXMLDoc);
	struct DrawContext dc;
	dc.UploadTexture = &UploadTextureMock;
	hAtlas atlas = At_LoadAtlas(root, &dc);
	if (atlas == NULL_HANDLE)
	{
		return 1;
	}
	struct BinarySerializer bs;
	BS_CreateForSave(args.outPath, &bs);
	At_SerializeAtlas(&bs, &atlas, &dc);
	BS_Finish(&bs);
	return 0;
}