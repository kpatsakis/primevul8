serialNumberAndIssuerSerialPretty(
	Syntax *syntax,
	struct berval *in,
	struct berval *out,
	void *ctx )
{
	struct berval sn, i, i_sn, ni = BER_BVNULL;
	char *p;
	int rc;

	assert( in != NULL );
	assert( out != NULL );

	Debug( LDAP_DEBUG_TRACE, ">>> serialNumberAndIssuerSerialPretty: <%s>\n",
		in->bv_val, 0, 0 );

	rc = serialNumberAndIssuerSerialCheck( in, &sn, &i, &i_sn, ctx );
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
	out->bv_len = STRLENOF("{ serialNumber , issuer { baseCertificateID { issuer { directoryName:rdnSequence:\"\" }, serial  } } }")
		+ sn.bv_len + ni.bv_len + i_sn.bv_len;
	out->bv_val = slap_sl_malloc( out->bv_len + 1, ctx );

	if ( out->bv_val == NULL ) {
		out->bv_len = 0;
		rc = LDAP_OTHER;
		goto done;
	}

	p = out->bv_val;
	p = lutil_strcopy( p, "{ serialNumber " );
	p = lutil_strbvcopy( p, &sn );
	p = lutil_strcopy( p, ", issuer { baseCertificateID { issuer { directoryName:rdnSequence:\"" );
	p = lutil_strbvcopy( p, &ni );
	p = lutil_strcopy( p, "\" }, serial " );
	p = lutil_strbvcopy( p, &i_sn );
	p = lutil_strcopy( p, " } } }" );

	assert( p == &out->bv_val[out->bv_len] );

done:;
	Debug( LDAP_DEBUG_TRACE, "<<< serialNumberAndIssuerSerialPretty: <%s> => <%s>\n",
		in->bv_val, rc == LDAP_SUCCESS ? out->bv_val : "(err)", 0 );

	slap_sl_free( ni.bv_val, ctx );

	return rc; 
}