SetModPtr(parser_t *pThis, modInfo_t *pMod)
{
	ISOBJ_TYPE_assert(pThis, parser);
	assert(pMod != NULL);
	assert(pThis->pModule == NULL);
	pThis->pModule = pMod;
	return RS_RET_OK;
}