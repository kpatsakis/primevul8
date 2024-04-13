ldap_pvt_tls_set_option( LDAP *ld, int option, void *arg )
{
	struct ldapoptions *lo;

	if( ld != NULL ) {
		assert( LDAP_VALID( ld ) );

		if( !LDAP_VALID( ld ) ) {
			return LDAP_OPT_ERROR;
		}

		lo = &ld->ld_options;

	} else {
		/* Get pointer to global option structure */
		lo = LDAP_INT_GLOBAL_OPT();   
		if ( lo == NULL ) {
			return LDAP_NO_MEMORY;
		}
	}

	switch( option ) {
	case LDAP_OPT_X_TLS:
		if ( !arg ) return -1;

		switch( *(int *) arg ) {
		case LDAP_OPT_X_TLS_NEVER:
		case LDAP_OPT_X_TLS_DEMAND:
		case LDAP_OPT_X_TLS_ALLOW:
		case LDAP_OPT_X_TLS_TRY:
		case LDAP_OPT_X_TLS_HARD:
			if (lo != NULL) {
				lo->ldo_tls_mode = *(int *)arg;
			}

			return 0;
		}
		return -1;

	case LDAP_OPT_X_TLS_CTX:
		if ( lo->ldo_tls_ctx )
			ldap_pvt_tls_ctx_free( lo->ldo_tls_ctx );
		lo->ldo_tls_ctx = arg;
		tls_ctx_ref( lo->ldo_tls_ctx );
		return 0;
	case LDAP_OPT_X_TLS_CONNECT_CB:
		lo->ldo_tls_connect_cb = (LDAP_TLS_CONNECT_CB *)arg;
		return 0;
	case LDAP_OPT_X_TLS_CONNECT_ARG:
		lo->ldo_tls_connect_arg = arg;
		return 0;
	case LDAP_OPT_X_TLS_CACERTFILE:
		if ( lo->ldo_tls_cacertfile ) LDAP_FREE( lo->ldo_tls_cacertfile );
		lo->ldo_tls_cacertfile = (arg && *(char *)arg) ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;
	case LDAP_OPT_X_TLS_CACERTDIR:
		if ( lo->ldo_tls_cacertdir ) LDAP_FREE( lo->ldo_tls_cacertdir );
		lo->ldo_tls_cacertdir = (arg && *(char *)arg) ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;
	case LDAP_OPT_X_TLS_CERTFILE:
		if ( lo->ldo_tls_certfile ) LDAP_FREE( lo->ldo_tls_certfile );
		lo->ldo_tls_certfile = (arg && *(char *)arg) ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;
	case LDAP_OPT_X_TLS_KEYFILE:
		if ( lo->ldo_tls_keyfile ) LDAP_FREE( lo->ldo_tls_keyfile );
		lo->ldo_tls_keyfile = (arg && *(char *)arg) ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;
	case LDAP_OPT_X_TLS_DHFILE:
		if ( lo->ldo_tls_dhfile ) LDAP_FREE( lo->ldo_tls_dhfile );
		lo->ldo_tls_dhfile = (arg && *(char *)arg) ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;
	case LDAP_OPT_X_TLS_ECNAME:
		if ( lo->ldo_tls_ecname ) LDAP_FREE( lo->ldo_tls_ecname );
		lo->ldo_tls_ecname = (arg && *(char *)arg) ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;
	case LDAP_OPT_X_TLS_CRLFILE:	/* GnuTLS only */
		if ( lo->ldo_tls_crlfile ) LDAP_FREE( lo->ldo_tls_crlfile );
		lo->ldo_tls_crlfile = (arg && *(char *)arg) ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;
	case LDAP_OPT_X_TLS_REQUIRE_CERT:
		if ( !arg ) return -1;
		switch( *(int *) arg ) {
		case LDAP_OPT_X_TLS_NEVER:
		case LDAP_OPT_X_TLS_DEMAND:
		case LDAP_OPT_X_TLS_ALLOW:
		case LDAP_OPT_X_TLS_TRY:
		case LDAP_OPT_X_TLS_HARD:
			lo->ldo_tls_require_cert = * (int *) arg;
			return 0;
		}
		return -1;
	case LDAP_OPT_X_TLS_REQUIRE_SAN:
		if ( !arg ) return -1;
		switch( *(int *) arg ) {
		case LDAP_OPT_X_TLS_NEVER:
		case LDAP_OPT_X_TLS_DEMAND:
		case LDAP_OPT_X_TLS_ALLOW:
		case LDAP_OPT_X_TLS_TRY:
		case LDAP_OPT_X_TLS_HARD:
			lo->ldo_tls_require_san = * (int *) arg;
			return 0;
		}
		return -1;
#ifdef HAVE_OPENSSL_CRL
	case LDAP_OPT_X_TLS_CRLCHECK:	/* OpenSSL only */
		if ( !arg ) return -1;
		switch( *(int *) arg ) {
		case LDAP_OPT_X_TLS_CRL_NONE:
		case LDAP_OPT_X_TLS_CRL_PEER:
		case LDAP_OPT_X_TLS_CRL_ALL:
			lo->ldo_tls_crlcheck = * (int *) arg;
			return 0;
		}
		return -1;
#endif
	case LDAP_OPT_X_TLS_CIPHER_SUITE:
		if ( lo->ldo_tls_ciphersuite ) LDAP_FREE( lo->ldo_tls_ciphersuite );
		lo->ldo_tls_ciphersuite = (arg && *(char *)arg) ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;

	case LDAP_OPT_X_TLS_PROTOCOL_MIN:
		if ( !arg ) return -1;
		lo->ldo_tls_protocol_min = *(int *)arg;
		return 0;
	case LDAP_OPT_X_TLS_RANDOM_FILE:
		if ( ld != NULL )
			return -1;
		if ( lo->ldo_tls_randfile ) LDAP_FREE (lo->ldo_tls_randfile );
		lo->ldo_tls_randfile = (arg && *(char *)arg) ? LDAP_STRDUP( (char *) arg ) : NULL;
		break;
	case LDAP_OPT_X_TLS_NEWCTX:
		if ( !arg ) return -1;
		if ( lo->ldo_tls_ctx )
			ldap_pvt_tls_ctx_free( lo->ldo_tls_ctx );
		lo->ldo_tls_ctx = NULL;
		return ldap_int_tls_init_ctx( lo, *(int *)arg );
	default:
		return -1;
	}
	return 0;
}