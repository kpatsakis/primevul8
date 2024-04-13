psf_allocate (void)
{	SF_PRIVATE * psf ;

	if ((psf = calloc (1, sizeof (SF_PRIVATE))) == NULL)
		return	NULL ;

	if ((psf->header.ptr = calloc (1, INITAL_HEADER_SIZE)) == NULL)
	{	free (psf) ;
		return	NULL ;
		} ;
	psf->header.len = INITAL_HEADER_SIZE ;

	return psf ;
} /* psf_allocate */