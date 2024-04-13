issuerAndThisUpdateNormalize(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *in,
	struct berval *out,
	void *ctx )
{
	struct berval i, ni, tu, tu2;
	char sbuf[STRLENOF("YYYYmmddHHMMSSZ") + 1];
	char *p;
	int rc;

	assert( in != NULL );
	assert( out != NULL );

	Debug( LDAP_DEBUG_TRACE, ">>> issuerAndThisUpdateNormalize: <%s>\n",
		in->bv_val, 0, 0 );

	rc = issuerAndThisUpdateCheck( in, &i, &tu, ctx );
	if ( rc ) {
		return rc;
	}

	rc = dnNormalize( usage, syntax, mr, &i, &ni, ctx );

	if ( in->bv_val[0] == '{' && in->bv_val[in->bv_len-1] == '}' ) {
		slap_sl_free( i.bv_val, ctx );
	}

	tu2.bv_val = sbuf;
	tu2.bv_len = sizeof( sbuf );
	if ( rc || checkTime( &tu, &tu2 ) ) {
		return LDAP_INVALID_SYNTAX;
	}

	out->bv_len = STRLENOF( "{ issuer rdnSequence:\"\", thisUpdate \"\" }" )
		+ ni.bv_len + tu2.bv_len;
	out->bv_val = slap_sl_malloc( out->bv_len + 1, ctx );

	if ( out->bv_val == NULL ) {
		out->bv_len = 0;
		rc = LDAP_OTHER;
		goto func_leave;
	}

	p = out->bv_val;

	p = lutil_strcopy( p, "{ issuer rdnSequence:\"" /*}*/ );
	p = lutil_strbvcopy( p, &ni );
	p = lutil_strcopy( p, "\", thisUpdate \"" );
	p = lutil_strbvcopy( p, &tu2 );
	p = lutil_strcopy( p, /*{*/ "\" }" );

	assert( p == &out->bv_val[out->bv_len] );

func_leave:
	Debug( LDAP_DEBUG_TRACE, "<<< issuerAndThisUpdateNormalize: <%s> => <%s>\n",
		in->bv_val, rc == LDAP_SUCCESS ? out->bv_val : "(err)", 0 );

	slap_sl_free( ni.bv_val, ctx );

	return rc;
}