BOOL nego_set_cookie(rdpNego* nego, char* cookie)
{
	if (nego->cookie)
	{
		free(nego->cookie);
		nego->cookie = NULL;
	}

	if (!cookie)
		return TRUE;

	nego->cookie = _strdup(cookie);

	if (!nego->cookie)
		return FALSE;

	return TRUE;
}