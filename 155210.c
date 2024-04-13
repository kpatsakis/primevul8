ldap_pvt_tls_ctx_free ( void *c )
{
	if ( !c ) return;
	tls_imp->ti_ctx_free( c );
}