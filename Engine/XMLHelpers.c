#include "XMLHelpers.h"
#include "xml.h"
#include "AssertLib.h"

void XML_AttributeNameToBuffer(struct xml_node* pXMLNode, char* pOutBuf, int attributeI, size_t maxBufLen)
{
	struct xml_string* pStr = xml_node_attribute_name(pXMLNode, attributeI);
	size_t length = xml_string_length(pStr);
	EASSERT(length < maxBufLen);
	xml_string_copy(pStr, pOutBuf, length);
	pOutBuf[length] = 0;
}

void XML_AttributeContentToBuffer(struct xml_node* pXMLNode, char* pOutBuf, int attributeI, size_t maxBufLen)
{
	struct xml_string* pStr = xml_node_attribute_content(pXMLNode, attributeI);
	size_t length = xml_string_length(pStr);
	EASSERT(length < maxBufLen);
	xml_string_copy(pStr, pOutBuf, length);
	pOutBuf[length] = 0;
}

void XML_NodeName(struct xml_node* pXMLNode, char* pOutBuf, size_t maxBufLen)
{
	struct xml_string* pstr = xml_node_name(pXMLNode);
	int len = xml_string_length(pstr);
	EASSERT(len < maxBufLen);
	xml_string_copy(pstr, pOutBuf, len);
	pOutBuf[len] = '\0';

}
