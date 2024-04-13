SetDoPRIParsing(parser_t *pThis, int bDoIt)
{
	ISOBJ_TYPE_assert(pThis, parser);
	pThis->bDoPRIParsing = bDoIt;
	return RS_RET_OK;
}