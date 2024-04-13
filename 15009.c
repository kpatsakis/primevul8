UUIDNormalize(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *normalized,
	void *ctx )
{
	unsigned char octet = '\0';
	int i;
	int j;

	if ( SLAP_MR_IS_DENORMALIZE( usage ) ) {
		/* NOTE: must be a normalized UUID */
		if( val->bv_len != 16 )
			return LDAP_INVALID_SYNTAX;

		normalized->bv_val = slap_sl_malloc( LDAP_LUTIL_UUIDSTR_BUFSIZE, ctx );
		normalized->bv_len = lutil_uuidstr_from_normalized( val->bv_val,
			val->bv_len, normalized->bv_val, LDAP_LUTIL_UUIDSTR_BUFSIZE );
		if( normalized->bv_len != STRLENOF( "BADBADBA-DBAD-0123-4567-BADBADBADBAD" ) )
			return LDAP_INVALID_SYNTAX;

		return LDAP_SUCCESS;
	}

	normalized->bv_len = 16;
	normalized->bv_val = slap_sl_malloc( normalized->bv_len + 1, ctx );

	for( i=0, j=0; i<36; i++ ) {
		unsigned char nibble;
		if( val->bv_val[i] == '-' ) {
			continue;

		} else if( ASCII_DIGIT( val->bv_val[i] ) ) {
			nibble = val->bv_val[i] - '0';

		} else if( ASCII_HEXLOWER( val->bv_val[i] ) ) {
			nibble = val->bv_val[i] - ('a'-10);

		} else if( ASCII_HEXUPPER( val->bv_val[i] ) ) {
			nibble = val->bv_val[i] - ('A'-10);

		} else {
			slap_sl_free( normalized->bv_val, ctx );
			BER_BVZERO( normalized );
			return LDAP_INVALID_SYNTAX;
		}

		if( j & 1 ) {
			octet |= nibble;
			normalized->bv_val[j>>1] = octet;
		} else {
			octet = nibble << 4;
		}
		j++;
	}

	normalized->bv_val[normalized->bv_len] = 0;
	return LDAP_SUCCESS;
}