tls_init(tls_impl *impl )
{
	static int tls_initialized = 0;

	if ( !tls_initialized++ ) {
#ifdef LDAP_R_COMPILE
		ldap_pvt_thread_mutex_init( &tls_def_ctx_mutex );
#endif
	}

	if ( impl->ti_inited++ ) return 0;

#ifdef LDAP_R_COMPILE
	impl->ti_thr_init();
#endif
	return impl->ti_tls_init();
}