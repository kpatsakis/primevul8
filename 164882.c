void *_af_realloc (void *p, size_t size)
{
	if (size <= 0)
	{
		_af_error(AF_BAD_MALLOC, "bad memory allocation size request %d", size);
		return NULL;
	}

	p = realloc(p, size);

	if (p == NULL)
	{
		_af_error(AF_BAD_MALLOC, "allocation of %d bytes failed", size);
		return NULL;
	}

	return p;
}