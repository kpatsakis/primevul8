certificateExactNormalize(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *normalized,
	void *ctx )
{
	BerElementBuffer berbuf;
	BerElement *ber = (BerElement *)&berbuf;
	ber_tag_t tag;
	ber_len_t len;
	ber_int_t i;
	char serialbuf2[SLAP_SN_BUFLEN];
	struct berval sn, sn2 = BER_BVNULL;
	struct berval issuer_dn = BER_BVNULL, bvdn;
	char *p;
	int rc = LDAP_INVALID_SYNTAX;

	assert( val != NULL );

	Debug( LDAP_DEBUG_TRACE, ">>> certificateExactNormalize: <%p, %lu>\n",
		val->bv_val, val->bv_len, 0 );

	if ( BER_BVISEMPTY( val ) ) goto done;

	if ( SLAP_MR_IS_VALUE_OF_ASSERTION_SYNTAX(usage) ) {
		return serialNumberAndIssuerNormalize( 0, NULL, NULL, val, normalized, ctx );
	}

	assert( SLAP_MR_IS_VALUE_OF_ATTRIBUTE_SYNTAX(usage) != 0 );

	ber_init2( ber, val, LBER_USE_DER );
	tag = ber_skip_tag( ber, &len );	/* Signed Sequence */
	tag = ber_skip_tag( ber, &len );	/* Sequence */
	tag = ber_peek_tag( ber, &len );	/* Optional version? */
	if ( tag == SLAP_X509_OPT_C_VERSION ) {
		tag = ber_skip_tag( ber, &len );
		tag = ber_get_int( ber, &i );	/* version */
	}

	/* NOTE: move the test here from certificateValidate,
	 * so that we can validate certs with serial longer
	 * than sizeof(ber_int_t) */
	tag = ber_skip_tag( ber, &len );	/* serial */
	sn.bv_len = len;
	sn.bv_val = (char *)ber->ber_ptr;
	sn2.bv_val = serialbuf2;
	sn2.bv_len = sizeof(serialbuf2);
	if ( slap_bin2hex( &sn, &sn2, ctx ) ) {
		rc = LDAP_INVALID_SYNTAX;
		goto done;
	}
	ber_skip_data( ber, len );

	tag = ber_skip_tag( ber, &len );	/* SignatureAlg */
	ber_skip_data( ber, len );
	tag = ber_peek_tag( ber, &len );	/* IssuerDN */
	if ( len ) {
		len = ber_ptrlen( ber );
		bvdn.bv_val = val->bv_val + len;
		bvdn.bv_len = val->bv_len - len;

		rc = dnX509normalize( &bvdn, &issuer_dn );
		if ( rc != LDAP_SUCCESS ) {
			rc = LDAP_INVALID_SYNTAX;
			goto done;
		}
	}

	normalized->bv_len = STRLENOF( "{ serialNumber , issuer rdnSequence:\"\" }" )
		+ sn2.bv_len + issuer_dn.bv_len;
	normalized->bv_val = ch_malloc( normalized->bv_len + 1 );

	p = normalized->bv_val;

	p = lutil_strcopy( p, "{ serialNumber " /*}*/ );
	p = lutil_strbvcopy( p, &sn2 );
	p = lutil_strcopy( p, ", issuer rdnSequence:\"" );
	p = lutil_strbvcopy( p, &issuer_dn );
	p = lutil_strcopy( p, /*{*/ "\" }" );

	rc = LDAP_SUCCESS;

done:
	Debug( LDAP_DEBUG_TRACE, "<<< certificateExactNormalize: <%p, %lu> => <%s>\n",
		val->bv_val, val->bv_len, rc == LDAP_SUCCESS ? normalized->bv_val : "(err)" );

	if ( issuer_dn.bv_val ) ber_memfree( issuer_dn.bv_val );
	if ( sn2.bv_val != serialbuf2 ) ber_memfree_x( sn2.bv_val, ctx );

	return rc;
}