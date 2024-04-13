issuerAndThisUpdateValidate(
	Syntax *syntax,
	struct berval *in )
{
	int rc;
	struct berval i, tu;

	Debug( LDAP_DEBUG_TRACE, ">>> issuerAndThisUpdateValidate: <%s>\n",
		in->bv_val, 0, 0 );

	rc = issuerAndThisUpdateCheck( in, &i, &tu, NULL );
	if ( rc ) {
		goto done;
	}

	/* validate DN -- doesn't handle double dquote */ 
	rc = dnValidate( NULL, &i );
	if ( rc ) {
		rc = LDAP_INVALID_SYNTAX;

	} else if ( checkTime( &tu, NULL ) ) {
		rc = LDAP_INVALID_SYNTAX;
	}

	if ( in->bv_val[0] == '{' && in->bv_val[in->bv_len-1] == '}' ) {
		slap_sl_free( i.bv_val, NULL );
	}

	Debug( LDAP_DEBUG_TRACE, "<<< issuerAndThisUpdateValidate: <%s> err=%d\n",
		in->bv_val, rc, 0 );

done:;
	return rc;
}