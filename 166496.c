printParserList(parserList_t *pList)
{
	while(pList != NULL) {
		dbgprintf("parser: %s\n", pList->pParser->pName);
		pList = pList->pNext;
	}
}