psf_log_SF_INFO (SF_PRIVATE *psf)
{	psf_log_printf (psf, "---------------------------------\n") ;

	psf_log_printf (psf, " Sample rate :   %d\n", psf->sf.samplerate) ;
	if (psf->sf.frames == SF_COUNT_MAX)
		psf_log_printf (psf, " Frames      :   unknown\n") ;
	else
		psf_log_printf (psf, " Frames      :   %D\n", psf->sf.frames) ;
	psf_log_printf (psf, " Channels    :   %d\n", psf->sf.channels) ;

	psf_log_printf (psf, " Format      :   0x%X\n", psf->sf.format) ;
	psf_log_printf (psf, " Sections    :   %d\n", psf->sf.sections) ;
	psf_log_printf (psf, " Seekable    :   %s\n", psf->sf.seekable ? "TRUE" : "FALSE") ;

	psf_log_printf (psf, "---------------------------------\n") ;
} /* psf_dump_SFINFO */