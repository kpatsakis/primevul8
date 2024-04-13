AddParserToList(parserList_t **ppListRoot, parser_t *pParser)
{
	parserList_t *pThis;
	parserList_t *pTail;
	DEFiRet;

	CHKmalloc(pThis = MALLOC(sizeof(parserList_t)));
	pThis->pParser = pParser;
	pThis->pNext = NULL;

	if(*ppListRoot == NULL) {
		pThis->pNext = *ppListRoot;
		*ppListRoot = pThis;
	} else {
		/* find tail first */
		for(pTail = *ppListRoot ; pTail->pNext != NULL ; pTail = pTail->pNext)
			/* just search, do nothing else */;
		/* add at tail */
		pTail->pNext = pThis;
	}
DBGPRINTF("DDDDD: added parser '%s' to list %p\n", pParser->pName, ppListRoot);
finalize_it:
	RETiRet;
}