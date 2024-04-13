wavex_write_fmt_chunk (SF_PRIVATE *psf)
{	WAVLIKE_PRIVATE	*wpriv ;
	int subformat, fmt_size ;

	if ((wpriv = psf->container_data) == NULL)
		return SFE_INTERNAL ;

	subformat = SF_CODEC (psf->sf.format) ;

	/* initial section (same for all, it appears) */
	switch (subformat)
	{	case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
		case SF_FORMAT_FLOAT :
		case SF_FORMAT_DOUBLE :
		case SF_FORMAT_ULAW :
		case SF_FORMAT_ALAW :
			fmt_size = 2 + 2 + 4 + 4 + 2 + 2 + 2 + 2 + 4 + 4 + 2 + 2 + 8 ;

			/* fmt : format, channels, samplerate */
			psf_binheader_writef (psf, "4224", BHW4 (fmt_size), BHW2 (WAVE_FORMAT_EXTENSIBLE), BHW2 (psf->sf.channels), BHW4 (psf->sf.samplerate)) ;
			/*  fmt : bytespersec */
			psf_binheader_writef (psf, "4", BHW4 (psf->sf.samplerate * psf->bytewidth * psf->sf.channels)) ;
			/*  fmt : blockalign, bitwidth */
			psf_binheader_writef (psf, "22", BHW2 (psf->bytewidth * psf->sf.channels), BHW2 (psf->bytewidth * 8)) ;

			/* cbSize 22 is sizeof (WAVEFORMATEXTENSIBLE) - sizeof (WAVEFORMATEX) */
			psf_binheader_writef (psf, "2", BHW2 (22)) ;

			/* wValidBitsPerSample, for our use same as bitwidth as we use it fully */
			psf_binheader_writef (psf, "2", BHW2 (psf->bytewidth * 8)) ;

			/* For an Ambisonic file set the channel mask to zero.
			** Otherwise use a default based on the channel count.
			*/
			if (wpriv->wavex_ambisonic != SF_AMBISONIC_NONE)
				psf_binheader_writef (psf, "4", BHW4 (0)) ;
			else if (wpriv->wavex_channelmask != 0)
				psf_binheader_writef (psf, "4", BHW4 (wpriv->wavex_channelmask)) ;
			else
			{	/*
				** Ok some liberty is taken here to use the most commonly used channel masks
				** instead of "no mapping". If you really want to use "no mapping" for 8 channels and less
				** please don't use wavex. (otherwise we'll have to create a new SF_COMMAND)
				*/
				switch (psf->sf.channels)
				{	case 1 :	/* center channel mono */
						psf_binheader_writef (psf, "4", BHW4 (0x4)) ;
						break ;

					case 2 :	/* front left and right */
						psf_binheader_writef (psf, "4", BHW4 (0x1 | 0x2)) ;
						break ;

					case 4 :	/* Quad */
						psf_binheader_writef (psf, "4", BHW4 (0x1 | 0x2 | 0x10 | 0x20)) ;
						break ;

					case 6 :	/* 5.1 */
						psf_binheader_writef (psf, "4", BHW4 (0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20)) ;
						break ;

					case 8 :	/* 7.1 */
						psf_binheader_writef (psf, "4", BHW4 (0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20 | 0x40 | 0x80)) ;
						break ;

					default :	/* 0 when in doubt , use direct out, ie NO mapping*/
						psf_binheader_writef (psf, "4", BHW4 (0x0)) ;
						break ;
					} ;
				} ;
			break ;

		case SF_FORMAT_MS_ADPCM : /* Todo, GUID exists might have different header as per wav_write_header */
		default :
			return SFE_UNIMPLEMENTED ;
		} ;

	/* GUID section, different for each */

	switch (subformat)
	{	case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
			wavlike_write_guid (psf, wpriv->wavex_ambisonic == SF_AMBISONIC_NONE ?
						&MSGUID_SUBTYPE_PCM : &MSGUID_SUBTYPE_AMBISONIC_B_FORMAT_PCM) ;
			break ;

		case SF_FORMAT_FLOAT :
		case SF_FORMAT_DOUBLE :
			wavlike_write_guid (psf, wpriv->wavex_ambisonic == SF_AMBISONIC_NONE ?
						&MSGUID_SUBTYPE_IEEE_FLOAT : &MSGUID_SUBTYPE_AMBISONIC_B_FORMAT_IEEE_FLOAT) ;
			break ;

		case SF_FORMAT_ULAW :
			wavlike_write_guid (psf, &MSGUID_SUBTYPE_MULAW) ;
			break ;

		case SF_FORMAT_ALAW :
			wavlike_write_guid (psf, &MSGUID_SUBTYPE_ALAW) ;
			break ;

#if 0
		/* This is dead code due to return in previous switch statement. */
		case SF_FORMAT_MS_ADPCM : /* todo, GUID exists */
			wavlike_write_guid (psf, &MSGUID_SUBTYPE_MS_ADPCM) ;
			break ;
			return SFE_UNIMPLEMENTED ;
#endif

		default : return SFE_UNIMPLEMENTED ;
		} ;

	psf_binheader_writef (psf, "tm48", BHWm (fact_MARKER), BHW4 (4), BHW8 (psf->sf.frames)) ;

	return 0 ;
} /* wavex_write_fmt_chunk */