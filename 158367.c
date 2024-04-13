serialNumberAndIssuerPretty(
	Syntax *syntax,
	struct berval *in,
	struct berval *out,
	void *ctx )
{
	int rc;
	struct berval sn, i, ni = BER_BVNULL;
	char *p;

	assert( in != NULL );
	assert( out != NULL );

	BER_BVZERO( out );

	Debug( LDAP_DEBUG_TRACE, ">>> serialNumberAndIssuerPretty: <%s>\n",
		in->bv_val, 0, 0 );

	rc = serialNumberAndIssuerCheck( in, &sn, &i, ctx );
	if ( rc ) {
		goto done;
	}

	rc = dnPretty( syntax, &i, &ni, ctx );

	if ( in->bv_val[0] == '{' && in->bv_val[in->bv_len-1] == '}' ) {
		slap_sl_free( i.bv_val, ctx );
	}

	if ( rc ) {
		rc = LDAP_INVALID_SYNTAX;
		goto done;
	}

	/* make room from sn + "$" */
	out->bv_len = STRLENOF("{ serialNumber , issuer rdnSequence:\"\" }")
		+ sn.bv_len + ni.bv_len;
	out->bv_val = slap_sl_malloc( out->bv_len + 1, ctx );

	if ( out->bv_val == NULL ) {
		out->bv_len = 0;
		rc = LDAP_OTHER;
		goto done;
	}

	p = out->bv_val;
	p = lutil_strcopy( p, "{ serialNumber " /*}*/ );
	p = lutil_strbvcopy( p, &sn );
	p = lutil_strcopy( p, ", issuer rdnSequence:\"" );
	p = lutil_strbvcopy( p, &ni );
	p = lutil_strcopy( p, /*{*/ "\" }" );

	assert( p == &out->bv_val[out->bv_len] );

done:;
	Debug( LDAP_DEBUG_TRACE, "<<< serialNumberAndIssuerPretty: <%s> => <%s>\n",
		in->bv_val, rc == LDAP_SUCCESS ? out->bv_val : "(err)", 0 );

	slap_sl_free( ni.bv_val, ctx );

	return LDAP_SUCCESS; 
}