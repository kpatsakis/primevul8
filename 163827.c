wav_read_acid_chunk (SF_PRIVATE *psf, uint32_t chunklen)
{	char buffer [512] ;
	uint32_t bytesread = 0 ;
	int	beats, flags ;
	short rootnote, q1, meter_denom, meter_numer ;
	float q2, tempo ;

	chunklen += (chunklen & 1) ;

	bytesread += psf_binheader_readf (psf, "422f", &flags, &rootnote, &q1, &q2) ;

	snprintf (buffer, sizeof (buffer), "%f", q2) ;

	psf_log_printf (psf, "  Flags     : 0x%04x (%s,%s,%s,%s,%s)\n", flags,
			(flags & 0x01) ? "OneShot" : "Loop",
			(flags & 0x02) ? "RootNoteValid" : "RootNoteInvalid",
			(flags & 0x04) ? "StretchOn" : "StretchOff",
			(flags & 0x08) ? "DiskBased" : "RAMBased",
			(flags & 0x10) ? "??On" : "??Off") ;

	psf_log_printf (psf, "  Root note : 0x%x\n  ????      : 0x%04x\n  ????      : %s\n",
				rootnote, q1, buffer) ;

	bytesread += psf_binheader_readf (psf, "422f", &beats, &meter_denom, &meter_numer, &tempo) ;
	snprintf (buffer, sizeof (buffer), "%f", tempo) ;
	psf_log_printf (psf, "  Beats     : %d\n  Meter     : %d/%d\n  Tempo     : %s\n",
				beats, meter_numer, meter_denom, buffer) ;

	psf_binheader_readf (psf, "j", chunklen - bytesread) ;

	if ((psf->loop_info = calloc (1, sizeof (SF_LOOP_INFO))) == NULL)
		return SFE_MALLOC_FAILED ;

	psf->loop_info->time_sig_num	= meter_numer ;
	psf->loop_info->time_sig_den	= meter_denom ;
	psf->loop_info->loop_mode		= (flags & 0x01) ? SF_LOOP_NONE : SF_LOOP_FORWARD ;
	psf->loop_info->num_beats		= beats ;
	psf->loop_info->bpm				= tempo ;
	psf->loop_info->root_key		= (flags & 0x02) ? rootnote : -1 ;

	return 0 ;
} /* wav_read_acid_chunk */