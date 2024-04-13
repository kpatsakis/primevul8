alloc_handle( void *ctx_arg, int is_server )
{
	tls_ctx	*ctx;
	tls_session	*ssl;

	if ( ctx_arg ) {
		ctx = ctx_arg;
	} else {
		struct ldapoptions *lo = LDAP_INT_GLOBAL_OPT();   
		if ( ldap_pvt_tls_init_def_ctx( is_server ) < 0 ) return NULL;
		ctx = lo->ldo_tls_ctx;
	}

	ssl = tls_imp->ti_session_new( ctx, is_server );
	if ( ssl == NULL ) {
		Debug( LDAP_DEBUG_ANY,"TLS: can't create ssl handle.\n",0,0,0);
		return NULL;
	}
	return ssl;
}