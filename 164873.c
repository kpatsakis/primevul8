void *_af_malloc (size_t size)
{
	void	*p;

	if (size <= 0)
	{
		_af_error(AF_BAD_MALLOC, "bad memory allocation size request %d", size);
		return NULL;
	}

	p = malloc(size);

#ifdef AF_DEBUG
	if (p)
		memset(p, 0xff, size);
#endif

	if (p == NULL)
	{
		_af_error(AF_BAD_MALLOC, "allocation of %d bytes failed", size);
		return NULL;
	}

	return p;
}