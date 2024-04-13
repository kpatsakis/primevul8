psf_f2s_array (const float *src, short *dest, int count, int normalize)
{	float 			normfact ;

	normfact = normalize ? (1.0 * 0x7FFF) : 1.0 ;
	while (--count >= 0)
		dest [count] = lrintf (src [count] * normfact) ;

	return ;
} /* psf_f2s_array */