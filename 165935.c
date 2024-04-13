psf_hexdump (const void *ptr, int len)
{	const char *data ;
	char	ascii [17] ;
	int		k, m ;

	if ((data = ptr) == NULL)
		return ;
	if (len <= 0)
		return ;

	puts ("") ;
	for (k = 0 ; k < len ; k += 16)
	{	memset (ascii, ' ', sizeof (ascii)) ;

		printf ("%08X: ", k) ;
		for (m = 0 ; m < 16 && k + m < len ; m++)
		{	printf (m == 8 ? " %02X " : "%02X ", data [k + m] & 0xFF) ;
			ascii [m] = psf_isprint (data [k + m]) ? data [k + m] : '.' ;
			} ;

		if (m <= 8) printf (" ") ;
		for ( ; m < 16 ; m++) printf ("   ") ;

		ascii [16] = 0 ;
		printf (" %s\n", ascii) ;
		} ;

	puts ("") ;
} /* psf_hexdump */