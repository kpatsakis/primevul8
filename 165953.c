psf_open_tmpfile (char * fname, size_t fnamelen)
{	const char * tmpdir ;
	FILE * file ;

	if (OS_IS_WIN32)
		tmpdir = getenv ("TEMP") ;
	else
	{	tmpdir = getenv ("TMPDIR") ;
		tmpdir = tmpdir == NULL ? "/tmp" : tmpdir ;
		} ;

	if (tmpdir && access (tmpdir, R_OK | W_OK | X_OK) == 0)
	{	snprintf (fname, fnamelen, "%s/%x%x-alac.tmp", tmpdir, psf_rand_int32 (), psf_rand_int32 ()) ;
		if ((file = fopen (fname, "wb+")) != NULL)
			return file ;
		} ;

	snprintf (fname, fnamelen, "%x%x-alac.tmp", psf_rand_int32 (), psf_rand_int32 ()) ;
	if ((file = fopen (fname, "wb+")) != NULL)
		return file ;

	memset (fname, 0, fnamelen) ;
	return NULL ;
} /* psf_open_tmpfile */