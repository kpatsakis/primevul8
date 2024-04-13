log_putchar (SF_PRIVATE *psf, char ch)
{	if (psf->parselog.indx < SIGNED_SIZEOF (psf->parselog.buf) - 1)
	{	psf->parselog.buf [psf->parselog.indx++] = ch ;
		psf->parselog.buf [psf->parselog.indx] = 0 ;
		} ;
	return ;
} /* log_putchar */