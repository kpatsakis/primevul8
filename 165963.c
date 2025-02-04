psf_f2i_array (const float *src, int *dest, int count, int normalize)
{	float 			normfact ;

	normfact = normalize ? (1.0 * 0x7FFFFFFF) : 1.0 ;
	while (--count >= 0)
		dest [count] = lrintf (src [count] * normfact) ;

	return ;
} /* psf_f2i_array */