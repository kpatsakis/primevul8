ldap_pvt_tls_get_my_dn( void *s, struct berval *dn, LDAPDN_rewrite_dummy *func, unsigned flags )
{
	tls_session *session = s;
	struct berval der_dn;
	int rc;

	rc = tls_imp->ti_session_my_dn( session, &der_dn );
	if ( rc == LDAP_SUCCESS )
		rc = ldap_X509dn2bv(&der_dn, dn, (LDAPDN_rewrite_func *)func, flags );
	return rc;
}