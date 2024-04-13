ldap_pvt_tls_destroy( void )
{
	struct ldapoptions *lo = LDAP_INT_GLOBAL_OPT();   

	ldap_int_tls_destroy( lo );

	tls_imp->ti_tls_destroy();
}