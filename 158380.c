certificateListExactNormalize(
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
	ber_int_t version;
	struct berval issuer_dn = BER_BVNULL, bvdn,
		thisUpdate, bvtu;
	char *p, tubuf[STRLENOF("YYYYmmddHHMMSSZ") + 1];
	int rc = LDAP_INVALID_SYNTAX;

	assert( val != NULL );

	Debug( LDAP_DEBUG_TRACE, ">>> certificateListExactNormalize: <%p, %lu>\n",
		val->bv_val, val->bv_len, 0 );

	if ( BER_BVISEMPTY( val ) ) goto done;

	if ( SLAP_MR_IS_VALUE_OF_ASSERTION_SYNTAX(usage) ) {
		return issuerAndThisUpdateNormalize( 0, NULL, NULL, val, normalized, ctx );
	}

	assert( SLAP_MR_IS_VALUE_OF_ATTRIBUTE_SYNTAX(usage) != 0 );

	ber_init2( ber, val, LBER_USE_DER );
	tag = ber_skip_tag( ber, &len );	/* Signed wrapper */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	tag = ber_skip_tag( ber, &len );	/* Sequence */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	tag = ber_peek_tag( ber, &len );
	/* Optional version */
	if ( tag == LBER_INTEGER ) {
		tag = ber_get_int( ber, &version );
		assert( tag == LBER_INTEGER );
		if ( version != SLAP_X509_V2 ) return LDAP_INVALID_SYNTAX;
	}
	tag = ber_skip_tag( ber, &len );	/* Signature Algorithm */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	ber_skip_data( ber, len );

	tag = ber_peek_tag( ber, &len );	/* IssuerDN */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	len = ber_ptrlen( ber );
	bvdn.bv_val = val->bv_val + len;
	bvdn.bv_len = val->bv_len - len;
	tag = ber_skip_tag( ber, &len );
	ber_skip_data( ber, len );

	tag = ber_skip_tag( ber, &len );	/* thisUpdate */
	/* Time is a CHOICE { UTCTime, GeneralizedTime } */
	if ( tag != SLAP_TAG_UTCTIME && tag != SLAP_TAG_GENERALIZEDTIME ) return LDAP_INVALID_SYNTAX;
	bvtu.bv_val = (char *)ber->ber_ptr;
	bvtu.bv_len = len;

	rc = dnX509normalize( &bvdn, &issuer_dn );
	if ( rc != LDAP_SUCCESS ) {
		rc = LDAP_INVALID_SYNTAX;
		goto done;
	}

	thisUpdate.bv_val = tubuf;
	thisUpdate.bv_len = sizeof(tubuf);
	if ( checkTime( &bvtu, &thisUpdate ) ) {
		rc = LDAP_INVALID_SYNTAX;
		goto done;
	}

	normalized->bv_len = STRLENOF( "{ issuer rdnSequence:\"\", thisUpdate \"\" }" )
		+ issuer_dn.bv_len + thisUpdate.bv_len;
	normalized->bv_val = ch_malloc( normalized->bv_len + 1 );

	p = normalized->bv_val;

	p = lutil_strcopy( p, "{ issuer rdnSequence:\"" );
	p = lutil_strbvcopy( p, &issuer_dn );
	p = lutil_strcopy( p, "\", thisUpdate \"" );
	p = lutil_strbvcopy( p, &thisUpdate );
	p = lutil_strcopy( p, /*{*/ "\" }" );

	rc = LDAP_SUCCESS;

done:
	Debug( LDAP_DEBUG_TRACE, "<<< certificateListExactNormalize: <%p, %lu> => <%s>\n",
		val->bv_val, val->bv_len, rc == LDAP_SUCCESS ? normalized->bv_val : "(err)" );

	if ( issuer_dn.bv_val ) ber_memfree( issuer_dn.bv_val );

	return rc;
}