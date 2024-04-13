psf_default_seek (SF_PRIVATE *psf, int UNUSED (mode), sf_count_t samples_from_start)
{	sf_count_t position, retval ;

	if (! (psf->blockwidth && psf->dataoffset >= 0))
	{	psf->error = SFE_BAD_SEEK ;
		return	PSF_SEEK_ERROR ;
		} ;

	if (! psf->sf.seekable)
	{	psf->error = SFE_NOT_SEEKABLE ;
		return	PSF_SEEK_ERROR ;
		} ;

	position = psf->dataoffset + psf->blockwidth * samples_from_start ;

	if ((retval = psf_fseek (psf, position, SEEK_SET)) != position)
	{	psf->error = SFE_SEEK_FAILED ;
		return PSF_SEEK_ERROR ;
		} ;

	return samples_from_start ;
} /* psf_default_seek */