psf_memset (void *s, int c, sf_count_t len)
{	char	*ptr ;
	int 	setcount ;

	ptr = (char *) s ;

	while (len > 0)
	{	setcount = (len > 0x10000000) ? 0x10000000 : (int) len ;

		memset (ptr, c, setcount) ;

		ptr += setcount ;
		len -= setcount ;
		} ;

	return s ;
} /* psf_memset */