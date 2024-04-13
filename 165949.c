psf_get_cues (SF_PRIVATE * psf, void * data, size_t datasize)
{
	if (psf->cues)
	{	uint32_t cue_count = (datasize - sizeof (uint32_t)) / sizeof (SF_CUE_POINT) ;

		cue_count = SF_MIN (cue_count, psf->cues->cue_count) ;
		memcpy (data, psf->cues, SF_CUES_VAR_SIZE (cue_count)) ;
		((SF_CUES*) data)->cue_count = cue_count ;
		} ;

	return ;
} /* psf_get_cues */