AddDfltParser(uchar *pName)
{
	parser_t *pParser;
	DEFiRet;

	CHKiRet(FindParser(&pParser, pName));
	CHKiRet(AddParserToList(&pDfltParsLst, pParser));
	DBGPRINTF("Parser '%s' added to default parser set.\n", pName);
	
finalize_it:
	RETiRet;
}