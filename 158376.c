csnValidate(
	Syntax *syntax,
	struct berval *in )
{
	struct berval	bv;
	char		*ptr;
	int		rc;

	assert( in != NULL );
	assert( !BER_BVISNULL( in ) );

	if ( BER_BVISEMPTY( in ) ) {
		return LDAP_INVALID_SYNTAX;
	}

	bv = *in;

	ptr = ber_bvchr( &bv, '#' );
	if ( ptr == NULL || ptr == &bv.bv_val[bv.bv_len] ) {
		return LDAP_INVALID_SYNTAX;
	}

	bv.bv_len = ptr - bv.bv_val;
	if ( bv.bv_len != STRLENOF( "YYYYmmddHHMMSS.uuuuuuZ" ) &&
		bv.bv_len != STRLENOF( "YYYYmmddHHMMSSZ" ) )
	{
		return LDAP_INVALID_SYNTAX;
	}

	rc = generalizedTimeValidate( NULL, &bv );
	if ( rc != LDAP_SUCCESS ) {
		return rc;
	}

	bv.bv_val = ptr + 1;
	bv.bv_len = in->bv_len - ( bv.bv_val - in->bv_val );

	ptr = ber_bvchr( &bv, '#' );
	if ( ptr == NULL || ptr == &in->bv_val[in->bv_len] ) {
		return LDAP_INVALID_SYNTAX;
	}

	bv.bv_len = ptr - bv.bv_val;
	if ( bv.bv_len != 6 ) {
		return LDAP_INVALID_SYNTAX;
	}

	rc = hexValidate( NULL, &bv );
	if ( rc != LDAP_SUCCESS ) {
		return rc;
	}

	bv.bv_val = ptr + 1;
	bv.bv_len = in->bv_len - ( bv.bv_val - in->bv_val );

	ptr = ber_bvchr( &bv, '#' );
	if ( ptr == NULL || ptr == &in->bv_val[in->bv_len] ) {
		return LDAP_INVALID_SYNTAX;
	}

	bv.bv_len = ptr - bv.bv_val;
	if ( bv.bv_len == 2 ) {
		/* tolerate old 2-digit replica-id */
		rc = hexValidate( NULL, &bv );

	} else {
		rc = sidValidate( NULL, &bv );
	}
	if ( rc != LDAP_SUCCESS ) {
		return rc;
	}

	bv.bv_val = ptr + 1;
	bv.bv_len = in->bv_len - ( bv.bv_val - in->bv_val );

	if ( bv.bv_len != 6 ) {
		return LDAP_INVALID_SYNTAX;
	}

	return hexValidate( NULL, &bv );
}