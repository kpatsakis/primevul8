psf_cues_alloc (uint32_t cue_count)
{	SF_CUES *pcues = calloc (1, SF_CUES_VAR_SIZE (cue_count)) ;

	pcues->cue_count = cue_count ;
	return pcues ;
} /* psf_cues_alloc */