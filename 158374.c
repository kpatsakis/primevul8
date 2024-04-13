octetStringSubstringsFilter (
	slap_mask_t use,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *prefix,
	void * assertedValue,
	BerVarray *keysp,
	void *ctx)
{
	SubstringsAssertion *sa;
	char pre;
	ber_len_t nkeys = 0;
	size_t slen, mlen, klen;
	BerVarray keys;
	HASH_CONTEXT HASHcontext;
	unsigned char HASHdigest[HASH_BYTES];
	struct berval *value;
	struct berval digest;

	sa = (SubstringsAssertion *) assertedValue;

	if( flags & SLAP_INDEX_SUBSTR_INITIAL &&
		!BER_BVISNULL( &sa->sa_initial ) &&
		sa->sa_initial.bv_len >= index_substr_if_minlen )
	{
		nkeys++;
		if ( sa->sa_initial.bv_len > index_substr_if_maxlen &&
			( flags & SLAP_INDEX_SUBSTR_ANY ))
		{
			nkeys += 1 + (sa->sa_initial.bv_len - index_substr_if_maxlen) / index_substr_any_step;
		}
	}

	if ( flags & SLAP_INDEX_SUBSTR_ANY && sa->sa_any != NULL ) {
		ber_len_t i;
		for( i=0; !BER_BVISNULL( &sa->sa_any[i] ); i++ ) {
			if( sa->sa_any[i].bv_len >= index_substr_any_len ) {
				/* don't bother accounting with stepping */
				nkeys += sa->sa_any[i].bv_len -
					( index_substr_any_len - 1 );
			}
		}
	}

	if( flags & SLAP_INDEX_SUBSTR_FINAL &&
		!BER_BVISNULL( &sa->sa_final ) &&
		sa->sa_final.bv_len >= index_substr_if_minlen )
	{
		nkeys++;
		if ( sa->sa_final.bv_len > index_substr_if_maxlen &&
			( flags & SLAP_INDEX_SUBSTR_ANY ))
		{
			nkeys += 1 + (sa->sa_final.bv_len - index_substr_if_maxlen) / index_substr_any_step;
		}
	}

	if( nkeys == 0 ) {
		*keysp = NULL;
		return LDAP_SUCCESS;
	}

	digest.bv_val = (char *)HASHdigest;
	digest.bv_len = sizeof(HASHdigest);

	slen = syntax->ssyn_oidlen;
	mlen = mr->smr_oidlen;

	keys = slap_sl_malloc( sizeof( struct berval ) * (nkeys+1), ctx );
	nkeys = 0;

	if( flags & SLAP_INDEX_SUBSTR_INITIAL &&
		!BER_BVISNULL( &sa->sa_initial ) &&
		sa->sa_initial.bv_len >= index_substr_if_minlen )
	{
		pre = SLAP_INDEX_SUBSTR_INITIAL_PREFIX;
		value = &sa->sa_initial;

		klen = index_substr_if_maxlen < value->bv_len
			? index_substr_if_maxlen : value->bv_len;

		hashPreset( &HASHcontext, prefix, pre, syntax, mr );
		hashIter( &HASHcontext, HASHdigest,
			(unsigned char *)value->bv_val, klen );
		ber_dupbv_x( &keys[nkeys++], &digest, ctx );

		/* If initial is too long and we have subany indexed, use it
		 * to match the excess...
		 */
		if (value->bv_len > index_substr_if_maxlen && (flags & SLAP_INDEX_SUBSTR_ANY))
		{
			ber_len_t j;
			pre = SLAP_INDEX_SUBSTR_PREFIX;
			hashPreset( &HASHcontext, prefix, pre, syntax, mr);
			for ( j=index_substr_if_maxlen-1; j <= value->bv_len - index_substr_any_len; j+=index_substr_any_step )
			{
				hashIter( &HASHcontext, HASHdigest,
					(unsigned char *)&value->bv_val[j], index_substr_any_len );
				ber_dupbv_x( &keys[nkeys++], &digest, ctx );
			}
		}
	}

	if( flags & SLAP_INDEX_SUBSTR_ANY && sa->sa_any != NULL ) {
		ber_len_t i, j;
		pre = SLAP_INDEX_SUBSTR_PREFIX;
		klen = index_substr_any_len;

		for( i=0; !BER_BVISNULL( &sa->sa_any[i] ); i++ ) {
			if( sa->sa_any[i].bv_len < index_substr_any_len ) {
				continue;
			}

			value = &sa->sa_any[i];

			hashPreset( &HASHcontext, prefix, pre, syntax, mr);
			for(j=0;
				j <= value->bv_len - index_substr_any_len;
				j += index_substr_any_step )
			{
				hashIter( &HASHcontext, HASHdigest,
					(unsigned char *)&value->bv_val[j], klen ); 
				ber_dupbv_x( &keys[nkeys++], &digest, ctx );
			}
		}
	}

	if( flags & SLAP_INDEX_SUBSTR_FINAL &&
		!BER_BVISNULL( &sa->sa_final ) &&
		sa->sa_final.bv_len >= index_substr_if_minlen )
	{
		pre = SLAP_INDEX_SUBSTR_FINAL_PREFIX;
		value = &sa->sa_final;

		klen = index_substr_if_maxlen < value->bv_len
			? index_substr_if_maxlen : value->bv_len;

		hashPreset( &HASHcontext, prefix, pre, syntax, mr );
		hashIter( &HASHcontext, HASHdigest,
			(unsigned char *)&value->bv_val[value->bv_len-klen], klen );
		ber_dupbv_x( &keys[nkeys++], &digest, ctx );

		/* If final is too long and we have subany indexed, use it
		 * to match the excess...
		 */
		if (value->bv_len > index_substr_if_maxlen && (flags & SLAP_INDEX_SUBSTR_ANY))
		{
			ber_len_t j;
			pre = SLAP_INDEX_SUBSTR_PREFIX;
			hashPreset( &HASHcontext, prefix, pre, syntax, mr);
			for ( j=0; j <= value->bv_len - index_substr_if_maxlen; j+=index_substr_any_step )
			{
				hashIter( &HASHcontext, HASHdigest,
					(unsigned char *)&value->bv_val[j], index_substr_any_len );
				ber_dupbv_x( &keys[nkeys++], &digest, ctx );
			}
		}
	}

	if( nkeys > 0 ) {
		BER_BVZERO( &keys[nkeys] );
		*keysp = keys;
	} else {
		ch_free( keys );
		*keysp = NULL;
	}

	return LDAP_SUCCESS;
}