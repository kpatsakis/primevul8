certificateValidate( Syntax *syntax, struct berval *in )
{
	BerElementBuffer berbuf;
	BerElement *ber = (BerElement *)&berbuf;
	ber_tag_t tag;
	ber_len_t len;
	ber_int_t version = SLAP_X509_V1;

	if ( BER_BVISNULL( in ) || BER_BVISEMPTY( in ))
		return LDAP_INVALID_SYNTAX;

	ber_init2( ber, in, LBER_USE_DER );
	tag = ber_skip_tag( ber, &len );	/* Signed wrapper */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	tag = ber_skip_tag( ber, &len );	/* Sequence */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	tag = ber_peek_tag( ber, &len );
	/* Optional version */
	if ( tag == SLAP_X509_OPT_C_VERSION ) {
		tag = ber_skip_tag( ber, &len );
		tag = ber_get_int( ber, &version );
		if ( tag != LBER_INTEGER ) return LDAP_INVALID_SYNTAX;
	}
	/* NOTE: don't try to parse Serial, because it might be longer
	 * than sizeof(ber_int_t); deferred to certificateExactNormalize() */
	tag = ber_skip_tag( ber, &len );	/* Serial */
	if ( tag != LBER_INTEGER ) return LDAP_INVALID_SYNTAX;
	ber_skip_data( ber, len );
	tag = ber_skip_tag( ber, &len );	/* Signature Algorithm */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	ber_skip_data( ber, len );
	tag = ber_skip_tag( ber, &len );	/* Issuer DN */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	ber_skip_data( ber, len );
	tag = ber_skip_tag( ber, &len );	/* Validity */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	ber_skip_data( ber, len );
	tag = ber_skip_tag( ber, &len );	/* Subject DN */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	ber_skip_data( ber, len );
	tag = ber_skip_tag( ber, &len );	/* Subject PublicKeyInfo */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	ber_skip_data( ber, len );
	tag = ber_skip_tag( ber, &len );
	if ( tag == SLAP_X509_OPT_C_ISSUERUNIQUEID ) {	/* issuerUniqueID */
		if ( version < SLAP_X509_V2 ) return LDAP_INVALID_SYNTAX;
		ber_skip_data( ber, len );
		tag = ber_skip_tag( ber, &len );
	}
	if ( tag == SLAP_X509_OPT_C_SUBJECTUNIQUEID ) {	/* subjectUniqueID */
		if ( version < SLAP_X509_V2 ) return LDAP_INVALID_SYNTAX;
		ber_skip_data( ber, len );
		tag = ber_skip_tag( ber, &len );
	}
	if ( tag == SLAP_X509_OPT_C_EXTENSIONS ) {	/* Extensions */
		if ( version < SLAP_X509_V3 ) return LDAP_INVALID_SYNTAX;
		tag = ber_skip_tag( ber, &len );
		if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
		ber_skip_data( ber, len );
		tag = ber_skip_tag( ber, &len );
	}
	/* signatureAlgorithm */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	ber_skip_data( ber, len );
	tag = ber_skip_tag( ber, &len );
	/* Signature */
	if ( tag != LBER_BITSTRING ) return LDAP_INVALID_SYNTAX; 
	ber_skip_data( ber, len );
	tag = ber_skip_tag( ber, &len );
	/* Must be at end now */
	if ( len || tag != LBER_DEFAULT ) return LDAP_INVALID_SYNTAX;
	return LDAP_SUCCESS;
}