attributeCertificateExactNormalize(
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
	char issuer_serialbuf[SLAP_SN_BUFLEN], serialbuf[SLAP_SN_BUFLEN];
	struct berval sn, i_sn, sn2 = BER_BVNULL, i_sn2 = BER_BVNULL;
	struct berval issuer_dn = BER_BVNULL, bvdn;
	char *p;
	int rc = LDAP_INVALID_SYNTAX;

	if ( BER_BVISEMPTY( val ) ) {
		return rc;
	}

	if ( SLAP_MR_IS_VALUE_OF_ASSERTION_SYNTAX(usage) ) {
		return serialNumberAndIssuerSerialNormalize( 0, NULL, NULL, val, normalized, ctx );
	}

	assert( SLAP_MR_IS_VALUE_OF_ATTRIBUTE_SYNTAX(usage) != 0 );

	ber_init2( ber, val, LBER_USE_DER );
	tag = ber_skip_tag( ber, &len );	/* Signed Sequence */
	tag = ber_skip_tag( ber, &len );	/* Sequence */
	tag = ber_skip_tag( ber, &len );	/* (Mandatory) version; must be v2(1) */
	ber_skip_data( ber, len );
	tag = ber_skip_tag( ber, &len );	/* Holder Sequence */
	ber_skip_data( ber, len );

	/* Issuer */
	tag = ber_skip_tag( ber, &len );	/* Sequence */
						/* issuerName (GeneralNames sequence; optional)? */
	tag = ber_skip_tag( ber, &len );	/* baseCertificateID (sequence; optional)? */
	tag = ber_skip_tag( ber, &len );	/* GeneralNames (sequence) */
	tag = ber_skip_tag( ber, &len );	/* directoryName (we only accept this form of GeneralName) */
	if ( tag != SLAP_X509_GN_DIRECTORYNAME ) { 
		return LDAP_INVALID_SYNTAX; 
	}
	tag = ber_peek_tag( ber, &len );	/* sequence of RDN */
	len = ber_ptrlen( ber );
	bvdn.bv_val = val->bv_val + len;
	bvdn.bv_len = val->bv_len - len;
	rc = dnX509normalize( &bvdn, &issuer_dn );
	if ( rc != LDAP_SUCCESS ) {
		rc = LDAP_INVALID_SYNTAX;
		goto done;
	}
	
	tag = ber_skip_tag( ber, &len );	/* sequence of RDN */
	ber_skip_data( ber, len ); 
	tag = ber_skip_tag( ber, &len );	/* serial number */
	if ( tag != LBER_INTEGER ) {
		rc = LDAP_INVALID_SYNTAX; 
		goto done;
	}
	i_sn.bv_val = (char *)ber->ber_ptr;
	i_sn.bv_len = len;
	i_sn2.bv_val = issuer_serialbuf;
	i_sn2.bv_len = sizeof(issuer_serialbuf);
	if ( slap_bin2hex( &i_sn, &i_sn2, ctx ) ) {
		rc = LDAP_INVALID_SYNTAX;
		goto done;
	}
	ber_skip_data( ber, len );

						/* issuerUID (bitstring; optional)? */
						/* objectDigestInfo (sequence; optional)? */

	tag = ber_skip_tag( ber, &len );	/* Signature (sequence) */
	ber_skip_data( ber, len );
	tag = ber_skip_tag( ber, &len );	/* serial number */ 
	if ( tag != LBER_INTEGER ) {
		rc = LDAP_INVALID_SYNTAX; 
		goto done;
	}
	sn.bv_val = (char *)ber->ber_ptr;
	sn.bv_len = len;
	sn2.bv_val = serialbuf;
	sn2.bv_len = sizeof(serialbuf);
	if ( slap_bin2hex( &sn, &sn2, ctx ) ) {
		rc = LDAP_INVALID_SYNTAX;
		goto done;
	}
	ber_skip_data( ber, len );

	normalized->bv_len = STRLENOF( "{ serialNumber , issuer { baseCertificateID { issuer { directoryName:rdnSequence:\"\" }, serial  } } }" )
		+ sn2.bv_len + issuer_dn.bv_len + i_sn2.bv_len;
	normalized->bv_val = ch_malloc( normalized->bv_len + 1 );

	p = normalized->bv_val;

	p = lutil_strcopy( p, "{ serialNumber " );
	p = lutil_strbvcopy( p, &sn2 );
	p = lutil_strcopy( p, ", issuer { baseCertificateID { issuer { directoryName:rdnSequence:\"" );
	p = lutil_strbvcopy( p, &issuer_dn );
	p = lutil_strcopy( p, "\" }, serial " );
	p = lutil_strbvcopy( p, &i_sn2 );
	p = lutil_strcopy( p, " } } }" );

	Debug( LDAP_DEBUG_TRACE, "attributeCertificateExactNormalize: %s\n",
		normalized->bv_val, NULL, NULL );

	rc = LDAP_SUCCESS;

done:
	if ( issuer_dn.bv_val ) ber_memfree( issuer_dn.bv_val );
	if ( i_sn2.bv_val != issuer_serialbuf ) ber_memfree_x( i_sn2.bv_val, ctx );
	if ( sn2.bv_val != serialbuf ) ber_memfree_x( sn2.bv_val, ctx );

	return rc;
}