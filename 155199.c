ldap_pvt_tls_get_peer_dn( void *s, struct berval *dn,
	LDAPDN_rewrite_dummy *func, unsigned flags )
{
	tls_session *session = s;
	struct berval bvdn;
	int rc;

	rc = tls_imp->ti_session_peer_dn( session, &bvdn );
	if ( rc ) return rc;

	rc = ldap_X509dn2bv( &bvdn, dn, 
			    (LDAPDN_rewrite_func *)func, flags);
	return rc;
}