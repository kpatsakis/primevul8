DestructParserList(parserList_t **ppListRoot)
{
	parserList_t *pParsLst;
	parserList_t *pParsLstDel;

	pParsLst = *ppListRoot;
	while(pParsLst != NULL) {
		pParsLstDel = pParsLst;
		pParsLst = pParsLst->pNext;
		free(pParsLstDel);
	}
	*ppListRoot = NULL;
	return RS_RET_OK;
}