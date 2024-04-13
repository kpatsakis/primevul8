psf_bump_header_allocation (SF_PRIVATE * psf, sf_count_t needed)
{
	sf_count_t newlen, smallest = INITAL_HEADER_SIZE ;
	void * ptr ;

	newlen = (needed > psf->header.len) ? 2 * SF_MAX (needed, smallest) : 2 * psf->header.len ;

	if (newlen > 100 * 1024)
	{	psf_log_printf (psf, "Request for header allocation of %D denied.\n", newlen) ;
		return 1 ;
		}

	if ((ptr = realloc (psf->header.ptr, newlen)) == NULL)
	{	psf_log_printf (psf, "realloc (%p, %D) failed\n", psf->header.ptr, newlen) ;
		psf->error = SFE_MALLOC_FAILED ;
		return 1 ;
		} ;

	/* Always zero-out new header memory to avoid un-initializer memory accesses. */
	if (newlen > psf->header.len)
		memset ((char *) ptr + psf->header.len, 0, newlen - psf->header.len) ;

	psf->header.ptr = ptr ;
	psf->header.len = newlen ;
	return 0 ;
} /* psf_bump_header_allocation */