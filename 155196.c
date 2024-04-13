ldap_start_tls_s ( LDAP *ld,
	LDAPControl **serverctrls,
	LDAPControl **clientctrls )
{
#ifndef HAVE_TLS
	return LDAP_NOT_SUPPORTED;
#else
	int rc;
	char *rspoid = NULL;
	struct berval *rspdata = NULL;

	/* XXYYZ: this initiates operation only on default connection! */

	if ( ldap_tls_inplace( ld ) ) {
		return LDAP_LOCAL_ERROR;
	}

	rc = ldap_extended_operation_s( ld, LDAP_EXOP_START_TLS,
		NULL, serverctrls, clientctrls, &rspoid, &rspdata );

	if ( rspoid != NULL ) {
		LDAP_FREE(rspoid);
	}

	if ( rspdata != NULL ) {
		ber_bvfree( rspdata );
	}

	if ( rc == LDAP_SUCCESS ) {
		rc = ldap_int_tls_start( ld, ld->ld_defconn, NULL );
	}

	return rc;
#endif
}