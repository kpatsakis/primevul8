serialNumberAndIssuerSerialNormalize(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *in,
	struct berval *out,
	void *ctx )
{
	struct berval i, ni = BER_BVNULL,
		sn, sn2 = BER_BVNULL, sn3 = BER_BVNULL,
		i_sn, i_sn2 = BER_BVNULL, i_sn3 = BER_BVNULL;
	char sbuf2[SLAP_SN_BUFLEN], i_sbuf2[SLAP_SN_BUFLEN],
		sbuf3[SLAP_SN_BUFLEN], i_sbuf3[SLAP_SN_BUFLEN];
	char *p;
	int rc;

	assert( in != NULL );
	assert( out != NULL );

	Debug( LDAP_DEBUG_TRACE, ">>> serialNumberAndIssuerSerialNormalize: <%s>\n",
		in->bv_val, 0, 0 );

	rc = serialNumberAndIssuerSerialCheck( in, &sn, &i, &i_sn, ctx );
	if ( rc ) {
		goto func_leave;
	}

	rc = dnNormalize( usage, syntax, mr, &i, &ni, ctx );

	if ( in->bv_val[0] == '{' && in->bv_val[in->bv_len-1] == '}' ) {
		slap_sl_free( i.bv_val, ctx );
	}

	if ( rc ) {
		rc = LDAP_INVALID_SYNTAX;
		goto func_leave;
	}

	/* Convert sn to canonical hex */
	sn2.bv_val = sbuf2;
	sn2.bv_len = sn.bv_len;
	if ( sn.bv_len > sizeof( sbuf2 ) ) {
		sn2.bv_val = slap_sl_malloc( sn.bv_len, ctx );
	}
	if ( lutil_str2bin( &sn, &sn2, ctx ) ) {
		rc = LDAP_INVALID_SYNTAX;
		goto func_leave;
	}

        /* Convert i_sn to canonical hex */
	i_sn2.bv_val = i_sbuf2;
	i_sn2.bv_len = i_sn.bv_len;
	if ( i_sn.bv_len > sizeof( i_sbuf2 ) ) {
		i_sn2.bv_val = slap_sl_malloc( i_sn.bv_len, ctx );
	}
	if ( lutil_str2bin( &i_sn, &i_sn2, ctx ) ) {
		rc = LDAP_INVALID_SYNTAX;
		goto func_leave;
	}

	sn3.bv_val = sbuf3;
	sn3.bv_len = sizeof(sbuf3);
	if ( slap_bin2hex( &sn2, &sn3, ctx ) ) {
		rc = LDAP_INVALID_SYNTAX;
		goto func_leave;
	}

	i_sn3.bv_val = i_sbuf3;
	i_sn3.bv_len = sizeof(i_sbuf3);
	if ( slap_bin2hex( &i_sn2, &i_sn3, ctx ) ) {
		rc = LDAP_INVALID_SYNTAX;
		goto func_leave;
	}

	out->bv_len = STRLENOF("{ serialNumber , issuer { baseCertificateID { issuer { directoryName:rdnSequence:\"\" }, serial  } } }")
		+ sn3.bv_len + ni.bv_len + i_sn3.bv_len;
	out->bv_val = slap_sl_malloc( out->bv_len + 1, ctx );

	if ( out->bv_val == NULL ) {
		out->bv_len = 0;
		rc = LDAP_OTHER;
		goto func_leave;
	}

	p = out->bv_val;

	p = lutil_strcopy( p, "{ serialNumber " );
	p = lutil_strbvcopy( p, &sn3 );
	p = lutil_strcopy( p, ", issuer { baseCertificateID { issuer { directoryName:rdnSequence:\"" );
	p = lutil_strbvcopy( p, &ni );
	p = lutil_strcopy( p, "\" }, serial " );
	p = lutil_strbvcopy( p, &i_sn3 );
	p = lutil_strcopy( p, " } } }" );

	assert( p == &out->bv_val[out->bv_len] );

func_leave:
	Debug( LDAP_DEBUG_TRACE, "<<< serialNumberAndIssuerSerialNormalize: <%s> => <%s>\n",
		in->bv_val, rc == LDAP_SUCCESS ? out->bv_val : "(err)", 0 );

	if ( sn2.bv_val != sbuf2 ) {
		slap_sl_free( sn2.bv_val, ctx );
	}

	if ( i_sn2.bv_val != i_sbuf2 ) {
		slap_sl_free( i_sn2.bv_val, ctx );
	}

	if ( sn3.bv_val != sbuf3 ) {
		slap_sl_free( sn3.bv_val, ctx );
	}

	if ( i_sn3.bv_val != i_sbuf3 ) {
		slap_sl_free( i_sn3.bv_val, ctx );
	}

	slap_sl_free( ni.bv_val, ctx );

	return rc;
}