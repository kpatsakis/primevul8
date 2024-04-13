destroyMasterParserList(void)
{
	parserList_t *pParsLst;
	parserList_t *pParsLstDel;

	pParsLst = pParsLstRoot;
	while(pParsLst != NULL) {
		parserDestruct(&pParsLst->pParser);
		pParsLstDel = pParsLst;
		pParsLst = pParsLst->pNext;
		free(pParsLstDel);
	}
}