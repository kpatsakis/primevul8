postalAddressNormalize(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *normalized,
	void *ctx )
{
	BerVarray lines = NULL, nlines = NULL;
	ber_len_t l, c;
	int rc = LDAP_SUCCESS;
	MatchingRule *xmr = NULL;
	char *p;

	if ( SLAP_MR_ASSOCIATED( mr, slap_schema.si_mr_caseIgnoreListMatch ) ) {
		xmr = slap_schema.si_mr_caseIgnoreMatch;

	} else {
		xmr = slap_schema.si_mr_caseExactMatch;
	}

	for ( l = 0, c = 0; c < val->bv_len; c++ ) {
		if ( val->bv_val[c] == '$' ) {
			l++;
		}
	}

	lines = slap_sl_calloc( sizeof( struct berval ), 2 * ( l + 2 ), ctx );
	nlines = &lines[l + 2];

	lines[0].bv_val = val->bv_val;
	for ( l = 0, c = 0; c < val->bv_len; c++ ) {
		if ( val->bv_val[c] == '$' ) {
			lines[l].bv_len = &val->bv_val[c] - lines[l].bv_val;
			l++;
			lines[l].bv_val = &val->bv_val[c + 1];
		}
	}
	lines[l].bv_len = &val->bv_val[c] - lines[l].bv_val;

	normalized->bv_len = c = l;

	for ( l = 0; l <= c; l++ ) {
		/* NOTE: we directly normalize each line,
		 * without unescaping the values, since the special
		 * values '\24' ('$') and '\5C' ('\') are not affected
		 * by normalization */
		if ( !lines[l].bv_len ) {
			nlines[l].bv_len = 0;
			nlines[l].bv_val = NULL;
			continue;
		}
		rc = UTF8StringNormalize( usage, NULL, xmr, &lines[l], &nlines[l], ctx );
		if ( rc != LDAP_SUCCESS ) {
			rc = LDAP_INVALID_SYNTAX;
			goto done;
		}

		normalized->bv_len += nlines[l].bv_len;
	}

	normalized->bv_val = slap_sl_malloc( normalized->bv_len + 1, ctx );

	p = normalized->bv_val;
	for ( l = 0; l <= c ; l++ ) {
		p = lutil_strbvcopy( p, &nlines[l] );
		*p++ = '$';
	}
	*--p = '\0';

	assert( p == &normalized->bv_val[normalized->bv_len] );

done:;
	if ( nlines != NULL ) {
		for ( l = 0; !BER_BVISNULL( &nlines[ l ] ); l++ ) {
			slap_sl_free( nlines[l].bv_val, ctx );
		}

		slap_sl_free( lines, ctx );
	}

	return rc;
}