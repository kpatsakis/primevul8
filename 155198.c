ldap_int_tls_config( LDAP *ld, int option, const char *arg )
{
	int i;

	switch( option ) {
	case LDAP_OPT_X_TLS_CACERTFILE:
	case LDAP_OPT_X_TLS_CACERTDIR:
	case LDAP_OPT_X_TLS_CERTFILE:
	case LDAP_OPT_X_TLS_KEYFILE:
	case LDAP_OPT_X_TLS_RANDOM_FILE:
	case LDAP_OPT_X_TLS_CIPHER_SUITE:
	case LDAP_OPT_X_TLS_DHFILE:
	case LDAP_OPT_X_TLS_ECNAME:
	case LDAP_OPT_X_TLS_CRLFILE:	/* GnuTLS only */
		return ldap_pvt_tls_set_option( ld, option, (void *) arg );

	case LDAP_OPT_X_TLS_REQUIRE_CERT:
	case LDAP_OPT_X_TLS_REQUIRE_SAN:
	case LDAP_OPT_X_TLS:
		i = -1;
		if ( strcasecmp( arg, "never" ) == 0 ) {
			i = LDAP_OPT_X_TLS_NEVER ;

		} else if ( strcasecmp( arg, "demand" ) == 0 ) {
			i = LDAP_OPT_X_TLS_DEMAND ;

		} else if ( strcasecmp( arg, "allow" ) == 0 ) {
			i = LDAP_OPT_X_TLS_ALLOW ;

		} else if ( strcasecmp( arg, "try" ) == 0 ) {
			i = LDAP_OPT_X_TLS_TRY ;

		} else if ( ( strcasecmp( arg, "hard" ) == 0 ) ||
			( strcasecmp( arg, "on" ) == 0 ) ||
			( strcasecmp( arg, "yes" ) == 0) ||
			( strcasecmp( arg, "true" ) == 0 ) )
		{
			i = LDAP_OPT_X_TLS_HARD ;
		}

		if (i >= 0) {
			return ldap_pvt_tls_set_option( ld, option, &i );
		}
		return -1;
	case LDAP_OPT_X_TLS_PROTOCOL_MIN: {
		char *next;
		long l;
		l = strtol( arg, &next, 10 );
		if ( l < 0 || l > 0xff || next == arg ||
			( *next != '\0' && *next != '.' ) )
			return -1;
		i = l << 8;
		if (*next == '.') {
			arg = next + 1;
			l = strtol( arg, &next, 10 );
			if ( l < 0 || l > 0xff || next == arg || *next != '\0' )
				return -1;
			i += l;
		}
		return ldap_pvt_tls_set_option( ld, option, &i );
		}
#ifdef HAVE_OPENSSL_CRL
	case LDAP_OPT_X_TLS_CRLCHECK:	/* OpenSSL only */
		i = -1;
		if ( strcasecmp( arg, "none" ) == 0 ) {
			i = LDAP_OPT_X_TLS_CRL_NONE ;
		} else if ( strcasecmp( arg, "peer" ) == 0 ) {
			i = LDAP_OPT_X_TLS_CRL_PEER ;
		} else if ( strcasecmp( arg, "all" ) == 0 ) {
			i = LDAP_OPT_X_TLS_CRL_ALL ;
		}
		if (i >= 0) {
			return ldap_pvt_tls_set_option( ld, option, &i );
		}
		return -1;
#endif
	}
	return -1;
}