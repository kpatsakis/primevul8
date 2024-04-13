nameUIDPretty(
	Syntax *syntax,
	struct berval *val,
	struct berval *out,
	void *ctx )
{
	assert( val != NULL );
	assert( out != NULL );


	Debug( LDAP_DEBUG_TRACE, ">>> nameUIDPretty: <%s>\n", val->bv_val, 0, 0 );

	if( BER_BVISEMPTY( val ) ) {
		ber_dupbv_x( out, val, ctx );

	} else if ( val->bv_len > SLAP_LDAPDN_MAXLEN ) {
		return LDAP_INVALID_SYNTAX;

	} else {
		int		rc;
		struct berval	dnval = *val;
		struct berval	uidval = BER_BVNULL;

		uidval.bv_val = strrchr( val->bv_val, '#' );
		if ( !BER_BVISNULL( &uidval ) ) {
			uidval.bv_val++;
			uidval.bv_len = val->bv_len - ( uidval.bv_val - val->bv_val );

			rc = bitStringValidate( NULL, &uidval );

			if ( rc == LDAP_SUCCESS ) {
				ber_dupbv_x( &dnval, val, ctx );
				uidval.bv_val--;
				dnval.bv_len -= ++uidval.bv_len;
				dnval.bv_val[dnval.bv_len] = '\0';

			} else {
				BER_BVZERO( &uidval );
			}
		}

		rc = dnPretty( syntax, &dnval, out, ctx );
		if ( dnval.bv_val != val->bv_val ) {
			slap_sl_free( dnval.bv_val, ctx );
		}
		if( rc != LDAP_SUCCESS ) {
			return rc;
		}

		if( !BER_BVISNULL( &uidval ) ) {
			char	*tmp;

			tmp = slap_sl_realloc( out->bv_val, out->bv_len 
				+ uidval.bv_len + 1,
				ctx );
			if( tmp == NULL ) {
				ber_memfree_x( out->bv_val, ctx );
				return LDAP_OTHER;
			}
			out->bv_val = tmp;
			memcpy( out->bv_val + out->bv_len, uidval.bv_val, uidval.bv_len );
			out->bv_len += uidval.bv_len;
			out->bv_val[out->bv_len] = '\0';
		}
	}

	Debug( LDAP_DEBUG_TRACE, "<<< nameUIDPretty: <%s>\n", out->bv_val, 0, 0 );

	return LDAP_SUCCESS;
}