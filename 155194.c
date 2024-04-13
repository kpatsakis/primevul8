ldap_int_tls_init_ctx( struct ldapoptions *lo, int is_server )
{
	int rc = 0;
	tls_impl *ti = tls_imp;
	struct ldaptls lts = lo->ldo_tls_info;

	if ( lo->ldo_tls_ctx )
		return 0;

	tls_init( ti );

	if ( is_server && !lts.lt_certfile && !lts.lt_keyfile &&
		!lts.lt_cacertfile && !lts.lt_cacertdir ) {
		/* minimum configuration not provided */
		return LDAP_NOT_SUPPORTED;
	}

#ifdef HAVE_EBCDIC
	/* This ASCII/EBCDIC handling is a real pain! */
	if ( lts.lt_ciphersuite ) {
		lts.lt_ciphersuite = LDAP_STRDUP( lts.lt_ciphersuite );
		__atoe( lts.lt_ciphersuite );
	}
	if ( lts.lt_cacertfile ) {
		lts.lt_cacertfile = LDAP_STRDUP( lts.lt_cacertfile );
		__atoe( lts.lt_cacertfile );
	}
	if ( lts.lt_certfile ) {
		lts.lt_certfile = LDAP_STRDUP( lts.lt_certfile );
		__atoe( lts.lt_certfile );
	}
	if ( lts.lt_keyfile ) {
		lts.lt_keyfile = LDAP_STRDUP( lts.lt_keyfile );
		__atoe( lts.lt_keyfile );
	}
	if ( lts.lt_crlfile ) {
		lts.lt_crlfile = LDAP_STRDUP( lts.lt_crlfile );
		__atoe( lts.lt_crlfile );
	}
	if ( lts.lt_cacertdir ) {
		lts.lt_cacertdir = LDAP_STRDUP( lts.lt_cacertdir );
		__atoe( lts.lt_cacertdir );
	}
	if ( lts.lt_dhfile ) {
		lts.lt_dhfile = LDAP_STRDUP( lts.lt_dhfile );
		__atoe( lts.lt_dhfile );
	}
	if ( lts.lt_ecname ) {
		lts.lt_ecname = LDAP_STRDUP( lts.lt_ecname );
		__atoe( lts.lt_ecname );
	}
#endif
	lo->ldo_tls_ctx = ti->ti_ctx_new( lo );
	if ( lo->ldo_tls_ctx == NULL ) {
		Debug( LDAP_DEBUG_ANY,
		   "TLS: could not allocate default ctx.\n",
			0,0,0);
		rc = -1;
		goto error_exit;
	}

	rc = ti->ti_ctx_init( lo, &lts, is_server );

error_exit:
	if ( rc < 0 && lo->ldo_tls_ctx != NULL ) {
		ldap_pvt_tls_ctx_free( lo->ldo_tls_ctx );
		lo->ldo_tls_ctx = NULL;
	}
#ifdef HAVE_EBCDIC
	LDAP_FREE( lts.lt_ciphersuite );
	LDAP_FREE( lts.lt_cacertfile );
	LDAP_FREE( lts.lt_certfile );
	LDAP_FREE( lts.lt_keyfile );
	LDAP_FREE( lts.lt_crlfile );
	LDAP_FREE( lts.lt_cacertdir );
	LDAP_FREE( lts.lt_dhfile );
	LDAP_FREE( lts.lt_ecname );
#endif
	return rc;
}