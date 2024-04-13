wav_write_fmt_chunk (SF_PRIVATE *psf)
{	int subformat, fmt_size, add_fact_chunk = 0 ;

	subformat = SF_CODEC (psf->sf.format) ;

	switch (subformat)
	{	case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
					fmt_size = 2 + 2 + 4 + 4 + 2 + 2 ;

					/* fmt : format, channels, samplerate */
					psf_binheader_writef (psf, "4224", BHW4 (fmt_size), BHW2 (WAVE_FORMAT_PCM), BHW2 (psf->sf.channels), BHW4 (psf->sf.samplerate)) ;
					/*  fmt : bytespersec */
					psf_binheader_writef (psf, "4", BHW4 (psf->sf.samplerate * psf->bytewidth * psf->sf.channels)) ;
					/*  fmt : blockalign, bitwidth */
					psf_binheader_writef (psf, "22", BHW2 (psf->bytewidth * psf->sf.channels), BHW2 (psf->bytewidth * 8)) ;
					break ;

		case SF_FORMAT_FLOAT :
		case SF_FORMAT_DOUBLE :
					fmt_size = 2 + 2 + 4 + 4 + 2 + 2 ;

					/* fmt : format, channels, samplerate */
					psf_binheader_writef (psf, "4224", BHW4 (fmt_size), BHW2 (WAVE_FORMAT_IEEE_FLOAT), BHW2 (psf->sf.channels), BHW4 (psf->sf.samplerate)) ;
					/*  fmt : bytespersec */
					psf_binheader_writef (psf, "4", BHW4 (psf->sf.samplerate * psf->bytewidth * psf->sf.channels)) ;
					/*  fmt : blockalign, bitwidth */
					psf_binheader_writef (psf, "22", BHW2 (psf->bytewidth * psf->sf.channels), BHW2 (psf->bytewidth * 8)) ;

					add_fact_chunk = SF_TRUE ;
					break ;

		case SF_FORMAT_ULAW :
					fmt_size = 2 + 2 + 4 + 4 + 2 + 2 + 2 ;

					/* fmt : format, channels, samplerate */
					psf_binheader_writef (psf, "4224", BHW4 (fmt_size), BHW2 (WAVE_FORMAT_MULAW), BHW2 (psf->sf.channels), BHW4 (psf->sf.samplerate)) ;
					/*  fmt : bytespersec */
					psf_binheader_writef (psf, "4", BHW4 (psf->sf.samplerate * psf->bytewidth * psf->sf.channels)) ;
					/*  fmt : blockalign, bitwidth, extrabytes */
					psf_binheader_writef (psf, "222", BHW2 (psf->bytewidth * psf->sf.channels), BHW2 (8), BHW2 (0)) ;

					add_fact_chunk = SF_TRUE ;
					break ;

		case SF_FORMAT_ALAW :
					fmt_size = 2 + 2 + 4 + 4 + 2 + 2 + 2 ;

					/* fmt : format, channels, samplerate */
					psf_binheader_writef (psf, "4224", BHW4 (fmt_size), BHW2 (WAVE_FORMAT_ALAW), BHW2 (psf->sf.channels), BHW4 (psf->sf.samplerate)) ;
					/*  fmt : bytespersec */
					psf_binheader_writef (psf, "4", BHW4 (psf->sf.samplerate * psf->bytewidth * psf->sf.channels)) ;
					/*  fmt : blockalign, bitwidth, extrabytes */
					psf_binheader_writef (psf, "222", BHW2 (psf->bytewidth * psf->sf.channels), BHW2 (8), BHW2 (0)) ;

					add_fact_chunk = SF_TRUE ;
					break ;

		/* Lite remove start */
		case SF_FORMAT_IMA_ADPCM :
					{	int blockalign, framesperblock, bytespersec ;

						blockalign		= wavlike_srate2blocksize (psf->sf.samplerate * psf->sf.channels) ;
						framesperblock	= 2 * (blockalign - 4 * psf->sf.channels) / psf->sf.channels + 1 ;
						bytespersec		= (psf->sf.samplerate * blockalign) / framesperblock ;

						/* fmt chunk. */
						fmt_size = 2 + 2 + 4 + 4 + 2 + 2 + 2 + 2 ;

						/* fmt : size, WAV format type, channels, samplerate, bytespersec */
						psf_binheader_writef (psf, "42244", BHW4 (fmt_size), BHW2 (WAVE_FORMAT_IMA_ADPCM),
									BHW2 (psf->sf.channels), BHW4 (psf->sf.samplerate), BHW4 (bytespersec)) ;

						/* fmt : blockalign, bitwidth, extrabytes, framesperblock. */
						psf_binheader_writef (psf, "2222", BHW2 (blockalign), BHW2 (4), BHW2 (2), BHW2 (framesperblock)) ;
						} ;

					add_fact_chunk = SF_TRUE ;
					break ;

		case SF_FORMAT_MS_ADPCM :
					{	int	blockalign, framesperblock, bytespersec, extrabytes ;

						blockalign		= wavlike_srate2blocksize (psf->sf.samplerate * psf->sf.channels) ;
						framesperblock	= 2 + 2 * (blockalign - 7 * psf->sf.channels) / psf->sf.channels ;
						bytespersec		= (psf->sf.samplerate * blockalign) / framesperblock ;

						/* fmt chunk. */
						extrabytes	= 2 + 2 + WAVLIKE_MSADPCM_ADAPT_COEFF_COUNT * (2 + 2) ;
						fmt_size	= 2 + 2 + 4 + 4 + 2 + 2 + 2 + extrabytes ;

						/* fmt : size, WAV format type, channels. */
						psf_binheader_writef (psf, "422", BHW4 (fmt_size), BHW2 (WAVE_FORMAT_MS_ADPCM), BHW2 (psf->sf.channels)) ;

						/* fmt : samplerate, bytespersec. */
						psf_binheader_writef (psf, "44", BHW4 (psf->sf.samplerate), BHW4 (bytespersec)) ;

						/* fmt : blockalign, bitwidth, extrabytes, framesperblock. */
						psf_binheader_writef (psf, "22222", BHW2 (blockalign), BHW2 (4), BHW2 (extrabytes), BHW2 (framesperblock), BHW2 (7)) ;

						wavlike_msadpcm_write_adapt_coeffs (psf) ;
						} ;

					add_fact_chunk = SF_TRUE ;
					break ;


		case SF_FORMAT_G721_32 :
					/* fmt chunk. */
					fmt_size = 2 + 2 + 4 + 4 + 2 + 2 + 2 + 2 ;

					/* fmt : size, WAV format type, channels, samplerate, bytespersec */
					psf_binheader_writef (psf, "42244", BHW4 (fmt_size), BHW2 (WAVE_FORMAT_G721_ADPCM),
								BHW2 (psf->sf.channels), BHW4 (psf->sf.samplerate), BHW4 (psf->sf.samplerate * psf->sf.channels / 2)) ;

					/* fmt : blockalign, bitwidth, extrabytes, auxblocksize. */
					psf_binheader_writef (psf, "2222", BHW2 (64), BHW2 (4), BHW2 (2), BHW2 (0)) ;

					add_fact_chunk = SF_TRUE ;
					break ;

		case SF_FORMAT_NMS_ADPCM_16 :
		case SF_FORMAT_NMS_ADPCM_24 :
		case SF_FORMAT_NMS_ADPCM_32 :
					{	int bytespersec, blockalign, bitwidth ;

						bitwidth 	= subformat == SF_FORMAT_NMS_ADPCM_16 ? 2 : subformat == SF_FORMAT_NMS_ADPCM_24 ? 3 : 4 ;
						blockalign 	= 20 * bitwidth + 2 ;
						bytespersec	= psf->sf.samplerate * blockalign / 160 ;

						/* fmt chunk. */
						fmt_size = 2 + 2 + 4 + 4 + 2 + 2 ;

						/* fmt : format, channels, samplerate */
						psf_binheader_writef (psf, "4224", BHW4 (fmt_size), BHW2 (WAVE_FORMAT_NMS_VBXADPCM),
									BHW2 (psf->sf.channels), BHW4 (psf->sf.samplerate)) ;
						/*  fmt : bytespersec, blockalign, bitwidth */
						psf_binheader_writef (psf, "422", BHW4 (bytespersec), BHW2 (blockalign), BHW2 (bitwidth)) ;

						add_fact_chunk = SF_TRUE ;
						break ;
						}

		/* Lite remove end */

		case SF_FORMAT_GSM610 :
					{	int	blockalign, framesperblock, bytespersec ;

						blockalign		= WAVLIKE_GSM610_BLOCKSIZE ;
						framesperblock	= WAVLIKE_GSM610_SAMPLES ;
						bytespersec		= (psf->sf.samplerate * blockalign) / framesperblock ;

						/* fmt chunk. */
						fmt_size = 2 + 2 + 4 + 4 + 2 + 2 + 2 + 2 ;

						/* fmt : size, WAV format type, channels. */
						psf_binheader_writef (psf, "422", BHW4 (fmt_size), BHW2 (WAVE_FORMAT_GSM610), BHW2 (psf->sf.channels)) ;

						/* fmt : samplerate, bytespersec. */
						psf_binheader_writef (psf, "44", BHW4 (psf->sf.samplerate), BHW4 (bytespersec)) ;

						/* fmt : blockalign, bitwidth, extrabytes, framesperblock. */
						psf_binheader_writef (psf, "2222", BHW2 (blockalign), BHW2 (0), BHW2 (2), BHW2 (framesperblock)) ;
						} ;

					add_fact_chunk = SF_TRUE ;
					break ;

		default : 	return SFE_UNIMPLEMENTED ;
		} ;

	if (add_fact_chunk)
		psf_binheader_writef (psf, "tm48", BHWm (fact_MARKER), BHW4 (4), BHW8 (psf->sf.frames)) ;

	return 0 ;
} /* wav_write_fmt_chunk */