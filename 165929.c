psf_cues_dup (const void * ptr)
{	const SF_CUES *pcues = ptr ;
	SF_CUES *pnew = psf_cues_alloc (pcues->cue_count) ;

	memcpy (pnew, pcues, SF_CUES_VAR_SIZE (pcues->cue_count)) ;
	return pnew ;
} /* psf_cues_dup */