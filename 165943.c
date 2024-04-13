psf_instrument_alloc (void)
{	SF_INSTRUMENT *instr ;

	instr = calloc (1, sizeof (SF_INSTRUMENT)) ;

	if (instr == NULL)
		return NULL ;

	/* Set non-zero default values. */
	instr->basenote = -1 ;
	instr->velocity_lo = -1 ;
	instr->velocity_hi = -1 ;
	instr->key_lo = -1 ;
	instr->key_hi = -1 ;

	return instr ;
} /* psf_instrument_alloc */