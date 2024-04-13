ldap_pvt_tls_check_hostname( LDAP *ld, void *s, const char *name_in )
{
	tls_session *session = s;

	if (ld->ld_options.ldo_tls_require_cert != LDAP_OPT_X_TLS_NEVER &&
	    ld->ld_options.ldo_tls_require_cert != LDAP_OPT_X_TLS_ALLOW) {
		ld->ld_errno = tls_imp->ti_session_chkhost( ld, session, name_in );
		if (ld->ld_errno != LDAP_SUCCESS) {
			return ld->ld_errno;
		}
	}

	return LDAP_SUCCESS;
}