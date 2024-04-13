parserConstructViaModAndName(modInfo_t *__restrict__ pMod, uchar *const __restrict__ pName, void *pInst)
{
	rsRetVal localRet;
	parser_t *pParser = NULL;
	DEFiRet;

	if(pInst == NULL && pMod->mod.pm.newParserInst != NULL) {
		/* this happens for the default instance on ModLoad time */
		CHKiRet(pMod->mod.pm.newParserInst(NULL, &pInst));
	}
	CHKiRet(parserConstruct(&pParser));
	/* check some features */
	localRet = pMod->isCompatibleWithFeature(sFEATUREAutomaticSanitazion);
	if(localRet == RS_RET_OK){
		pParser->bDoSanitazion = RSTRUE;
	}
	localRet = pMod->isCompatibleWithFeature(sFEATUREAutomaticPRIParsing);
	if(localRet == RS_RET_OK){
		CHKiRet(SetDoPRIParsing(pParser, RSTRUE));
	}

	CHKiRet(SetName(pParser, pName));
	CHKiRet(SetModPtr(pParser, pMod));
	pParser->pInst = pInst;
	CHKiRet(parserConstructFinalize(pParser));
finalize_it:
	if(iRet != RS_RET_OK)
		free(pParser);
	RETiRet;
}