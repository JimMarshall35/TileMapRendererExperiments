#ifndef XML_HELPERS_H
#define XML_HELPERS_H

void XML_AttributeNameToBuffer(struct xml_node* pXMLNode, char* pOutBuf, int attributeI, size_t maxBufLen);
void XML_AttributeContentToBuffer(struct xml_node* pXMLNode, char* pOutBuf, int attributeI, size_t maxBufLen);
void XML_NodeName(struct xml_node* pXMLNode, char* pOutBuf, size_t maxBufLen);

#endif