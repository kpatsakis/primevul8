ldap_pvt_tls_init_def_ctx( int is_server )
{
	struct ldapoptions *lo = LDAP_INT_GLOBAL_OPT();   
	int rc;
	LDAP_MUTEX_LOCK( &tls_def_ctx_mutex );
	rc = ldap_int_tls_init_ctx( lo, is_server );
	LDAP_MUTEX_UNLOCK( &tls_def_ctx_mutex );
	return rc;
}