FindParser(parser_t **ppParser, uchar *pName)
{
	parserList_t *pThis;
	DEFiRet;
	
	for(pThis = pParsLstRoot ; pThis != NULL ; pThis = pThis->pNext) {
		if(ustrcmp(pThis->pParser->pName, pName) == 0) {
			*ppParser = pThis->pParser;
			FINALIZE;	/* found it, iRet still eq. OK! */
		}
	}

	iRet = RS_RET_PARSER_NOT_FOUND;

finalize_it:
	RETiRet;
}