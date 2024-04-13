serialNumberAndIssuerNormalize(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *in,
	struct berval *out,
	void *ctx )
{
	struct berval sn, sn2, sn3, i, ni;
	char sbuf2[SLAP_SN_BUFLEN];
	char sbuf3[SLAP_SN_BUFLEN];
	char *p;
	int rc;

	assert( in != NULL );
	assert( out != NULL );

	Debug( LDAP_DEBUG_TRACE, ">>> serialNumberAndIssuerNormalize: <%s>\n",
		in->bv_val, 0, 0 );

	rc = serialNumberAndIssuerCheck( in, &sn, &i, ctx );
	if ( rc ) {
		return rc;
	}

	rc = dnNormalize( usage, syntax, mr, &i, &ni, ctx );

	if ( in->bv_val[0] == '{' && in->bv_val[in->bv_len-1] == '}' ) {
		slap_sl_free( i.bv_val, ctx );
	}

	if ( rc ) {
		return LDAP_INVALID_SYNTAX;
	}

	/* Convert sn to canonical hex */
	sn2.bv_val = sbuf2;
	if ( sn.bv_len > sizeof( sbuf2 ) ) {
		sn2.bv_val = slap_sl_malloc( sn.bv_len, ctx );
	}
	sn2.bv_len = sn.bv_len;
	sn3.bv_val = sbuf3;
	sn3.bv_len = sizeof(sbuf3);
	if ( lutil_str2bin( &sn, &sn2, ctx ) || slap_bin2hex( &sn2, &sn3, ctx ) ) {
		rc = LDAP_INVALID_SYNTAX;
		goto func_leave;
	}

	out->bv_len = STRLENOF( "{ serialNumber , issuer rdnSequence:\"\" }" )
		+ sn3.bv_len + ni.bv_len;
	out->bv_val = slap_sl_malloc( out->bv_len + 1, ctx );
	if ( out->bv_val == NULL ) {
		out->bv_len = 0;
		rc = LDAP_OTHER;
		goto func_leave;
	}

	p = out->bv_val;

	p = lutil_strcopy( p, "{ serialNumber " /*}*/ );
	p = lutil_strbvcopy( p, &sn3 );
	p = lutil_strcopy( p, ", issuer rdnSequence:\"" );
	p = lutil_strbvcopy( p, &ni );
	p = lutil_strcopy( p, /*{*/ "\" }" );

	assert( p == &out->bv_val[out->bv_len] );

func_leave:
	Debug( LDAP_DEBUG_TRACE, "<<< serialNumberAndIssuerNormalize: <%s> => <%s>\n",
		in->bv_val, rc == LDAP_SUCCESS ? out->bv_val : "(err)", 0 );

	if ( sn2.bv_val != sbuf2 ) {
		slap_sl_free( sn2.bv_val, ctx );
	}

	if ( sn3.bv_val != sbuf3 ) {
		slap_sl_free( sn3.bv_val, ctx );
	}

	slap_sl_free( ni.bv_val, ctx );

	return rc;
}