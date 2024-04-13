psf_strlcpy_crlf (char *dest, const char *src, size_t destmax, size_t srcmax)
{	/* Must be minus 2 so it can still expand a single trailing '\n' or '\r'. */
	char * destend = dest + destmax - 2 ;
	const char * srcend = src + srcmax ;

	while (dest < destend && src < srcend)
	{	if ((src [0] == '\r' && src [1] == '\n') || (src [0] == '\n' && src [1] == '\r'))
		{	*dest++ = '\r' ;
			*dest++ = '\n' ;
			src += 2 ;
			continue ;
			} ;

		if (src [0] == '\r')
		{	*dest++ = '\r' ;
			*dest++ = '\n' ;
			src += 1 ;
			continue ;
			} ;

		if (src [0] == '\n')
		{	*dest++ = '\r' ;
			*dest++ = '\n' ;
			src += 1 ;
			continue ;
			} ;

		*dest++ = *src++ ;
		} ;

	/* Make sure dest is terminated. */
	*dest = 0 ;
} /* psf_strlcpy_crlf */