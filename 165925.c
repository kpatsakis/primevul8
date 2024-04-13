psf_d2i_array (const double *src, int *dest, int count, int normalize)
{	double 			normfact ;

	normfact = normalize ? (1.0 * 0x7FFFFFFF) : 1.0 ;
	while (--count >= 0)
		dest [count] = lrint (src [count] * normfact) ;

	return ;
} /* psf_f2i_array */