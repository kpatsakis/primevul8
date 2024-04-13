static rsRetVal parserConstructFinalize(parser_t *pThis)
{
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, parser);
	CHKiRet(AddParserToList(&pParsLstRoot, pThis));
	DBGPRINTF("Parser '%s' added to list of available parsers.\n", pThis->pName);

finalize_it:
	RETiRet;
}