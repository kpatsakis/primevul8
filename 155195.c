ldap_int_tls_destroy( struct ldapoptions *lo )
{
	if ( lo->ldo_tls_ctx ) {
		ldap_pvt_tls_ctx_free( lo->ldo_tls_ctx );
		lo->ldo_tls_ctx = NULL;
	}

	if ( lo->ldo_tls_certfile ) {
		LDAP_FREE( lo->ldo_tls_certfile );
		lo->ldo_tls_certfile = NULL;
	}
	if ( lo->ldo_tls_keyfile ) {
		LDAP_FREE( lo->ldo_tls_keyfile );
		lo->ldo_tls_keyfile = NULL;
	}
	if ( lo->ldo_tls_dhfile ) {
		LDAP_FREE( lo->ldo_tls_dhfile );
		lo->ldo_tls_dhfile = NULL;
	}
	if ( lo->ldo_tls_ecname ) {
		LDAP_FREE( lo->ldo_tls_ecname );
		lo->ldo_tls_ecname = NULL;
	}
	if ( lo->ldo_tls_cacertfile ) {
		LDAP_FREE( lo->ldo_tls_cacertfile );
		lo->ldo_tls_cacertfile = NULL;
	}
	if ( lo->ldo_tls_cacertdir ) {
		LDAP_FREE( lo->ldo_tls_cacertdir );
		lo->ldo_tls_cacertdir = NULL;
	}
	if ( lo->ldo_tls_ciphersuite ) {
		LDAP_FREE( lo->ldo_tls_ciphersuite );
		lo->ldo_tls_ciphersuite = NULL;
	}
	if ( lo->ldo_tls_crlfile ) {
		LDAP_FREE( lo->ldo_tls_crlfile );
		lo->ldo_tls_crlfile = NULL;
	}
}