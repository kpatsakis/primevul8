psf_d2s_array (const double *src, short *dest, int count, int normalize)
{	double 			normfact ;

	normfact = normalize ? (1.0 * 0x7FFF) : 1.0 ;
	while (--count >= 0)
		dest [count] = lrint (src [count] * normfact) ;

	return ;
} /* psf_f2s_array */