wav_open	(SF_PRIVATE *psf)
{	WAVLIKE_PRIVATE * wpriv ;
	int	format, subformat, error, blockalign = 0, framesperblock = 0 ;

	if ((wpriv = calloc (1, sizeof (WAVLIKE_PRIVATE))) == NULL)
		return SFE_MALLOC_FAILED ;
	psf->container_data = wpriv ;

	wpriv->wavex_ambisonic = SF_AMBISONIC_NONE ;
	psf->strings.flags = SF_STR_ALLOW_START | SF_STR_ALLOW_END ;

	if (psf->file.mode == SFM_READ || (psf->file.mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = wav_read_header (psf, &blockalign, &framesperblock)))
			return error ;

		psf->next_chunk_iterator = wav_next_chunk_iterator ;
		psf->get_chunk_size = wav_get_chunk_size ;
		psf->get_chunk_data = wav_get_chunk_data ;
		} ;

	subformat = SF_CODEC (psf->sf.format) ;

	if (psf->file.mode == SFM_WRITE || psf->file.mode == SFM_RDWR)
	{	if (psf->is_pipe)
			return SFE_NO_PIPE_WRITE ;

		wpriv->wavex_ambisonic = SF_AMBISONIC_NONE ;

		format = SF_CONTAINER (psf->sf.format) ;
		if (format != SF_FORMAT_WAV && format != SF_FORMAT_WAVEX)
			return	SFE_BAD_OPEN_FORMAT ;

		psf->blockwidth = psf->bytewidth * psf->sf.channels ;

		/* RIFF WAVs are little-endian, RIFX WAVs are big-endian, default to little */
		psf->endian = SF_ENDIAN (psf->sf.format) ;
		if (CPU_IS_BIG_ENDIAN && psf->endian == SF_ENDIAN_CPU)
			psf->endian = SF_ENDIAN_BIG ;
		else if (psf->endian != SF_ENDIAN_BIG)
			psf->endian = SF_ENDIAN_LITTLE ;

		if (psf->file.mode != SFM_RDWR || psf->filelength < 44)
		{	psf->filelength = 0 ;
			psf->datalength = 0 ;
			psf->dataoffset = 0 ;
			psf->sf.frames = 0 ;
			} ;

		if (subformat == SF_FORMAT_IMA_ADPCM || subformat == SF_FORMAT_MS_ADPCM)
		{	blockalign = wavlike_srate2blocksize (psf->sf.samplerate * psf->sf.channels) ;
			framesperblock = -1 ; /* Corrected later. */
			} ;

		/* By default, add the peak chunk to floating point files. Default behaviour
		** can be switched off using sf_command (SFC_SET_PEAK_CHUNK, SF_FALSE).
		*/
		if (psf->file.mode == SFM_WRITE && (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE))
		{	if ((psf->peak_info = peak_info_calloc (psf->sf.channels)) == NULL)
				return SFE_MALLOC_FAILED ;
			psf->peak_info->peak_loc = SF_PEAK_START ;
			} ;

		psf->write_header	= wav_write_header ;
		psf->set_chunk		= wav_set_chunk ;
		} ;

	psf->container_close = wav_close ;
	psf->command = wav_command ;

	switch (subformat)
	{	case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
					error = pcm_init (psf) ;
					break ;

		case SF_FORMAT_ULAW :
					error = ulaw_init (psf) ;
					break ;

		case SF_FORMAT_ALAW :
					error = alaw_init (psf) ;
					break ;

		/* Lite remove start */
		case SF_FORMAT_FLOAT :
					error = float32_init (psf) ;
					break ;

		case SF_FORMAT_DOUBLE :
					error = double64_init (psf) ;
					break ;

		case SF_FORMAT_IMA_ADPCM :
					error = wavlike_ima_init (psf, blockalign, framesperblock) ;
					break ;

		case SF_FORMAT_MS_ADPCM :
					error = wavlike_msadpcm_init (psf, blockalign, framesperblock) ;
					break ;

		case SF_FORMAT_G721_32 :
					error = g72x_init (psf) ;
					break ;

		case SF_FORMAT_NMS_ADPCM_16 :
		case SF_FORMAT_NMS_ADPCM_24 :
		case SF_FORMAT_NMS_ADPCM_32 :
					error = nms_adpcm_init (psf) ;
					break ;

		/* Lite remove end */

		case SF_FORMAT_GSM610 :
					error = gsm610_init (psf) ;
					break ;

		default : 	return SFE_UNIMPLEMENTED ;
		} ;

	if (psf->file.mode == SFM_WRITE || (psf->file.mode == SFM_RDWR && psf->filelength == 0))
		return psf->write_header (psf, SF_FALSE) ;

	return error ;
} /* wav_open */