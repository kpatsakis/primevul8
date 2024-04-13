SetName(parser_t *pThis, uchar *name)
{
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, parser);
	assert(name != NULL);

	if(pThis->pName != NULL) {
		free(pThis->pName);
		pThis->pName = NULL;
	}

	CHKmalloc(pThis->pName = ustrdup(name));

finalize_it:
	RETiRet;
}