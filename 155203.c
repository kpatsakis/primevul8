ldap_start_tls( LDAP *ld,
	LDAPControl **serverctrls,
	LDAPControl **clientctrls,
	int *msgidp )
{
	return ldap_extended_operation( ld, LDAP_EXOP_START_TLS,
		NULL, serverctrls, clientctrls, msgidp );
}