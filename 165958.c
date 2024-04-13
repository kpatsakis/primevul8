psf_f2i_clip_array (const float *src, int *dest, int count, int normalize)
{	float			normfact, scaled_value ;

	normfact = normalize ? (8.0 * 0x10000000) : 1.0 ;

	while (--count >= 0)
	{	scaled_value = src [count] * normfact ;
		if (CPU_CLIPS_POSITIVE == 0 && scaled_value >= (1.0 * 0x7FFFFFFF))
		{	dest [count] = 0x7FFFFFFF ;
			continue ;
			} ;
		if (CPU_CLIPS_NEGATIVE == 0 && scaled_value <= (-8.0 * 0x10000000))
		{	dest [count] = 0x80000000 ;
			continue ;
			} ;

		dest [count] = lrintf (scaled_value) ;
		} ;

	return ;
} /* psf_f2i_clip_array */