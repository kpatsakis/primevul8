ldap_pvt_tls_get_strength( void *s )
{
	tls_session *session = s;

	return tls_imp->ti_session_strength( session );
}