psf_d2s_clip_array (const double *src, short *dest, int count, int normalize)
{	double			normfact, scaled_value ;

	normfact = normalize ? (1.0 * 0x8000) : 1.0 ;

	while (--count >= 0)
	{	scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFF))
		{	dest [count] = 0x7FFF ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x1000))
		{	dest [count] = 0x8000 ;
			continue ;
			} ;

		dest [count] = lrint (scaled_value) ;
		} ;

	return ;
} /* psf_d2s_clip_array */