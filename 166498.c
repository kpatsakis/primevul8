ParseMsg(smsg_t *pMsg)
{
	rsRetVal localRet = RS_RET_ERR;
	parserList_t *pParserList;
	parser_t *pParser;
	sbool bIsSanitized;
	sbool bPRIisParsed;
	static int iErrMsgRateLimiter = 0;
	DEFiRet;

	if(pMsg->iLenRawMsg == 0)
		ABORT_FINALIZE(RS_RET_EMPTY_MSG);

	CHKiRet(uncompressMessage(pMsg));

	/* we take the risk to print a non-sanitized string, because this is the best we can get
	 * (and that functionality is too important for debugging to drop it...).
	 */
	DBGPRINTF("msg parser: flags %x, from '%s', msg '%.60s'\n", pMsg->msgFlags,
		  (pMsg->msgFlags & NEEDS_DNSRESOL) ? UCHAR_CONSTANT("~NOTRESOLVED~") : getRcvFrom(pMsg),
		  pMsg->pszRawMsg);

	/* we now need to go through our list of parsers and see which one is capable of
	 * parsing the message. Note that the first parser that requires message sanitization
	 * will cause it to happen. After that, access to the unsanitized message is no
	 * loger possible.
	 */
	pParserList = ruleset.GetParserList(ourConf, pMsg);
	if(pParserList == NULL) {
		pParserList = pDfltParsLst;
	}
	DBGPRINTF("parse using parser list %p%s.\n", pParserList,
		  (pParserList == pDfltParsLst) ? " (the default list)" : "");

	bIsSanitized = RSFALSE;
	bPRIisParsed = RSFALSE;
	while(pParserList != NULL) {
		pParser = pParserList->pParser;
		if(pParser->bDoSanitazion && bIsSanitized == RSFALSE) {
			CHKiRet(SanitizeMsg(pMsg));
			if(pParser->bDoPRIParsing && bPRIisParsed == RSFALSE) {
				CHKiRet(ParsePRI(pMsg));
				bPRIisParsed = RSTRUE;
			}
			bIsSanitized = RSTRUE;
		}
		if(pParser->pModule->mod.pm.parse2 == NULL)
			localRet = pParser->pModule->mod.pm.parse(pMsg);
		else
			localRet = pParser->pModule->mod.pm.parse2(pParser->pInst, pMsg);
		DBGPRINTF("Parser '%s' returned %d\n", pParser->pName, localRet);
		if(localRet != RS_RET_COULD_NOT_PARSE)
			break;
		pParserList = pParserList->pNext;
	}

	/* We need to log a warning message and drop the message if we did not find a parser.
	 * Note that we log at most the first 1000 message, as this may very well be a problem
	 * that causes a message generation loop. We do not synchronize that counter, it doesn't
	 * matter if we log a handful messages more than we should...
	 */
	if(localRet != RS_RET_OK) {
		if(++iErrMsgRateLimiter > 1000) {
			errmsg.LogError(0, localRet, "Error: one message could not be processed by "
			 	"any parser, message is being discarded (start of raw msg: '%.50s')", 
				pMsg->pszRawMsg);
		}
		DBGPRINTF("No parser could process the message (state %d), we need to discard it.\n", localRet);
		ABORT_FINALIZE(localRet);
	}

	/* "finalize" message object */
	pMsg->msgFlags &= ~NEEDS_PARSING; /* this message is now parsed */

finalize_it:
	RETiRet;
}