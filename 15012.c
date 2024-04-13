IA5StringNormalize(
	slap_mask_t use,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *normalized,
	void *ctx )
{
	char *p, *q, *end;
	int casefold = !SLAP_MR_ASSOCIATED( mr,
		slap_schema.si_mr_caseExactIA5Match );

	assert( SLAP_MR_IS_VALUE_OF_SYNTAX( use ) != 0 );

	p = val->bv_val;
	end = val->bv_val + val->bv_len;

	/* Ignore initial whitespace */
	while ( p < end && ASCII_SPACE( *p ) ) p++;

	normalized->bv_len = p < end ? (val->bv_len - ( p - val->bv_val )) : 0;
	normalized->bv_val = slap_sl_malloc( normalized->bv_len + 1, ctx );
	AC_MEMCPY( normalized->bv_val, p, normalized->bv_len );
	normalized->bv_val[normalized->bv_len] = '\0';

	p = q = normalized->bv_val;

	while ( *p ) {
		if ( ASCII_SPACE( *p ) ) {
			*q++ = *p++;

			/* Ignore the extra whitespace */
			while ( ASCII_SPACE( *p ) ) {
				p++;
			}

		} else if ( casefold ) {
			/* Most IA5 rules require casefolding */
			*q++ = TOLOWER(*p); p++;

		} else {
			*q++ = *p++;
		}
	}

	assert( normalized->bv_val <= p );
	assert( q <= p );

	/*
	 * If the string ended in space, backup the pointer one
	 * position.  One is enough because the above loop collapsed
	 * all whitespace to a single space.
	 */
	if ( q > normalized->bv_val && ASCII_SPACE( q[-1] ) ) --q;

	/* null terminate */
	*q = '\0';

	normalized->bv_len = q - normalized->bv_val;

	return LDAP_SUCCESS;
}