wav_command (SF_PRIVATE *psf, int command, void * UNUSED (data), int datasize)
{	WAVLIKE_PRIVATE	*wpriv ;

	if ((wpriv = psf->container_data) == NULL)
		return SFE_INTERNAL ;

	switch (command)
	{	case SFC_WAVEX_SET_AMBISONIC :
			if ((SF_CONTAINER (psf->sf.format)) == SF_FORMAT_WAVEX)
			{	if (datasize == SF_AMBISONIC_NONE)
					wpriv->wavex_ambisonic = SF_AMBISONIC_NONE ;
				else if (datasize == SF_AMBISONIC_B_FORMAT)
					wpriv->wavex_ambisonic = SF_AMBISONIC_B_FORMAT ;
				else
					return 0 ;
				} ;
			return wpriv->wavex_ambisonic ;

		case SFC_WAVEX_GET_AMBISONIC :
			return wpriv->wavex_ambisonic ;

		case SFC_SET_CHANNEL_MAP_INFO :
			wpriv->wavex_channelmask = wavlike_gen_channel_mask (psf->channel_map, psf->sf.channels) ;
			return (wpriv->wavex_channelmask != 0) ;

		default :
			break ;
	} ;

	return 0 ;
} /* wav_command */