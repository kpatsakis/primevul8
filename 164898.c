void *_af_calloc (size_t nmemb, size_t size)
{
	void	*p;

	if (nmemb <= 0 || size <= 0)
	{
		_af_error(AF_BAD_MALLOC, "bad memory allocation size request "
			"%d elements of %d bytes each", nmemb, size);
		return NULL;
	}

	p = calloc(nmemb, size);

	if (p == NULL)
	{
		_af_error(AF_BAD_MALLOC, "allocation of %d bytes failed",
			nmemb*size);
		return NULL;
	}

	return p;
}