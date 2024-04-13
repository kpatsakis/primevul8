wav_write_tailer (SF_PRIVATE *psf)
{
	/* Reset the current header buffer length to zero. */
	psf->header.ptr [0] = 0 ;
	psf->header.indx = 0 ;

	if (psf->bytewidth > 0 && psf->sf.seekable == SF_TRUE)
	{	psf->datalength = psf->sf.frames * psf->bytewidth * psf->sf.channels ;
		psf->dataend = psf->dataoffset + psf->datalength ;
		} ;

	if (psf->dataend > 0)
		psf_fseek (psf, psf->dataend, SEEK_SET) ;
	else
		psf->dataend = psf_fseek (psf, 0, SEEK_END) ;

	if (psf->dataend & 1)
		psf_binheader_writef (psf, "z", BHWz (1)) ;

	/* Add a PEAK chunk if requested. */
	if (psf->peak_info != NULL && psf->peak_info->peak_loc == SF_PEAK_END)
		wavlike_write_peak_chunk (psf) ;

	if (psf->strings.flags & SF_STR_LOCATE_END)
		wavlike_write_strings (psf, SF_STR_LOCATE_END) ;

	/* Write the tailer. */
	if (psf->header.indx > 0)
		psf_fwrite (psf->header.ptr, psf->header.indx, 1, psf) ;

	return 0 ;
} /* wav_write_tailer */