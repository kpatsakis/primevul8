psf_sanitize_string (char * cptr, int len)
{
	do
	{
		len -- ;
		cptr [len] = psf_isprint (cptr [len]) ? cptr [len] : '.' ;
	}
	while (len > 0) ;
} /* psf_sanitize_string */