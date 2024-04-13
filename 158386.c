octetStringSubstringsIndexer(
	slap_mask_t use,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *prefix,
	BerVarray values,
	BerVarray *keysp,
	void *ctx )
{
	ber_len_t i, nkeys;
	size_t slen, mlen;
	BerVarray keys;

	HASH_CONTEXT HCany, HCini, HCfin;
	unsigned char HASHdigest[HASH_BYTES];
	struct berval digest;
	digest.bv_val = (char *)HASHdigest;
	digest.bv_len = sizeof(HASHdigest);

	nkeys = 0;

	for ( i = 0; !BER_BVISNULL( &values[i] ); i++ ) {
		/* count number of indices to generate */
		if( flags & SLAP_INDEX_SUBSTR_INITIAL ) {
			if( values[i].bv_len >= index_substr_if_maxlen ) {
				nkeys += index_substr_if_maxlen -
					(index_substr_if_minlen - 1);
			} else if( values[i].bv_len >= index_substr_if_minlen ) {
				nkeys += values[i].bv_len - (index_substr_if_minlen - 1);
			}
		}

		if( flags & SLAP_INDEX_SUBSTR_ANY ) {
			if( values[i].bv_len >= index_substr_any_len ) {
				nkeys += values[i].bv_len - (index_substr_any_len - 1);
			}
		}

		if( flags & SLAP_INDEX_SUBSTR_FINAL ) {
			if( values[i].bv_len >= index_substr_if_maxlen ) {
				nkeys += index_substr_if_maxlen -
					(index_substr_if_minlen - 1);
			} else if( values[i].bv_len >= index_substr_if_minlen ) {
				nkeys += values[i].bv_len - (index_substr_if_minlen - 1);
			}
		}
	}

	if( nkeys == 0 ) {
		/* no keys to generate */
		*keysp = NULL;
		return LDAP_SUCCESS;
	}

	keys = slap_sl_malloc( sizeof( struct berval ) * (nkeys+1), ctx );

	slen = syntax->ssyn_oidlen;
	mlen = mr->smr_oidlen;

	if ( flags & SLAP_INDEX_SUBSTR_ANY )
		hashPreset( &HCany, prefix, SLAP_INDEX_SUBSTR_PREFIX, syntax, mr );
	if( flags & SLAP_INDEX_SUBSTR_INITIAL )
		hashPreset( &HCini, prefix, SLAP_INDEX_SUBSTR_INITIAL_PREFIX, syntax, mr );
	if( flags & SLAP_INDEX_SUBSTR_FINAL )
		hashPreset( &HCfin, prefix, SLAP_INDEX_SUBSTR_FINAL_PREFIX, syntax, mr );

	nkeys = 0;
	for ( i = 0; !BER_BVISNULL( &values[i] ); i++ ) {
		ber_len_t j,max;

		if( ( flags & SLAP_INDEX_SUBSTR_ANY ) &&
			( values[i].bv_len >= index_substr_any_len ) )
		{
			max = values[i].bv_len - (index_substr_any_len - 1);

			for( j=0; j<max; j++ ) {
				hashIter( &HCany, HASHdigest,
					(unsigned char *)&values[i].bv_val[j],
					index_substr_any_len );
				ber_dupbv_x( &keys[nkeys++], &digest, ctx );
			}
		}

		/* skip if too short */ 
		if( values[i].bv_len < index_substr_if_minlen ) continue;

		max = index_substr_if_maxlen < values[i].bv_len
			? index_substr_if_maxlen : values[i].bv_len;

		for( j=index_substr_if_minlen; j<=max; j++ ) {

			if( flags & SLAP_INDEX_SUBSTR_INITIAL ) {
				hashIter( &HCini, HASHdigest,
					(unsigned char *)values[i].bv_val, j );
				ber_dupbv_x( &keys[nkeys++], &digest, ctx );
			}

			if( flags & SLAP_INDEX_SUBSTR_FINAL ) {
				hashIter( &HCfin, HASHdigest,
					(unsigned char *)&values[i].bv_val[values[i].bv_len-j], j );
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