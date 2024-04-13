ldap_install_tls( LDAP *ld )
{
#ifndef HAVE_TLS
	return LDAP_NOT_SUPPORTED;
#else
	if ( ldap_tls_inplace( ld ) ) {
		return LDAP_LOCAL_ERROR;
	}

	return ldap_int_tls_start( ld, ld->ld_defconn, NULL );
#endif
}