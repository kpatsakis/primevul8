ldap_pvt_tls_get_option( LDAP *ld, int option, void *arg )
{
	struct ldapoptions *lo;

	if( option == LDAP_OPT_X_TLS_PACKAGE ) {
		*(char **)arg = LDAP_STRDUP( tls_imp->ti_name );
		return 0;
	}

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
		*(int *)arg = lo->ldo_tls_mode;
		break;
	case LDAP_OPT_X_TLS_CTX:
		*(void **)arg = lo->ldo_tls_ctx;
		if ( lo->ldo_tls_ctx ) {
			tls_ctx_ref( lo->ldo_tls_ctx );
		}
		break;
	case LDAP_OPT_X_TLS_CACERTFILE:
		*(char **)arg = lo->ldo_tls_cacertfile ?
			LDAP_STRDUP( lo->ldo_tls_cacertfile ) : NULL;
		break;
	case LDAP_OPT_X_TLS_CACERTDIR:
		*(char **)arg = lo->ldo_tls_cacertdir ?
			LDAP_STRDUP( lo->ldo_tls_cacertdir ) : NULL;
		break;
	case LDAP_OPT_X_TLS_CERTFILE:
		*(char **)arg = lo->ldo_tls_certfile ?
			LDAP_STRDUP( lo->ldo_tls_certfile ) : NULL;
		break;
	case LDAP_OPT_X_TLS_KEYFILE:
		*(char **)arg = lo->ldo_tls_keyfile ?
			LDAP_STRDUP( lo->ldo_tls_keyfile ) : NULL;
		break;
	case LDAP_OPT_X_TLS_DHFILE:
		*(char **)arg = lo->ldo_tls_dhfile ?
			LDAP_STRDUP( lo->ldo_tls_dhfile ) : NULL;
		break;
	case LDAP_OPT_X_TLS_ECNAME:
		*(char **)arg = lo->ldo_tls_ecname ?
			LDAP_STRDUP( lo->ldo_tls_ecname ) : NULL;
		break;
	case LDAP_OPT_X_TLS_CRLFILE:	/* GnuTLS only */
		*(char **)arg = lo->ldo_tls_crlfile ?
			LDAP_STRDUP( lo->ldo_tls_crlfile ) : NULL;
		break;
	case LDAP_OPT_X_TLS_REQUIRE_CERT:
		*(int *)arg = lo->ldo_tls_require_cert;
		break;
	case LDAP_OPT_X_TLS_REQUIRE_SAN:
		*(int *)arg = lo->ldo_tls_require_san;
		break;
#ifdef HAVE_OPENSSL_CRL
	case LDAP_OPT_X_TLS_CRLCHECK:	/* OpenSSL only */
		*(int *)arg = lo->ldo_tls_crlcheck;
		break;
#endif
	case LDAP_OPT_X_TLS_CIPHER_SUITE:
		*(char **)arg = lo->ldo_tls_ciphersuite ?
			LDAP_STRDUP( lo->ldo_tls_ciphersuite ) : NULL;
		break;
	case LDAP_OPT_X_TLS_PROTOCOL_MIN:
		*(int *)arg = lo->ldo_tls_protocol_min;
		break;
	case LDAP_OPT_X_TLS_RANDOM_FILE:
		*(char **)arg = lo->ldo_tls_randfile ?
			LDAP_STRDUP( lo->ldo_tls_randfile ) : NULL;
		break;
	case LDAP_OPT_X_TLS_SSL_CTX: {
		void *retval = 0;
		if ( ld != NULL ) {
			LDAPConn *conn = ld->ld_defconn;
			if ( conn != NULL ) {
				Sockbuf *sb = conn->lconn_sb;
				retval = ldap_pvt_tls_sb_ctx( sb );
			}
		}
		*(void **)arg = retval;
		break;
	}
	case LDAP_OPT_X_TLS_CONNECT_CB:
		*(LDAP_TLS_CONNECT_CB **)arg = lo->ldo_tls_connect_cb;
		break;
	case LDAP_OPT_X_TLS_CONNECT_ARG:
		*(void **)arg = lo->ldo_tls_connect_arg;
		break;
	default:
		return -1;
	}
	return 0;
}