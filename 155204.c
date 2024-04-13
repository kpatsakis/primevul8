ldap_pvt_tls_accept( Sockbuf *sb, void *ctx_arg )
{
	int	err;
	tls_session	*ssl = NULL;

	if ( HAS_TLS( sb )) {
		ber_sockbuf_ctrl( sb, LBER_SB_OPT_GET_SSL, (void *)&ssl );
	} else {
		ssl = alloc_handle( ctx_arg, 1 );
		if ( ssl == NULL ) return -1;

#ifdef LDAP_DEBUG
		ber_sockbuf_add_io( sb, &ber_sockbuf_io_debug,
			LBER_SBIOD_LEVEL_TRANSPORT, (void *)"tls_" );
#endif
		ber_sockbuf_add_io( sb, tls_imp->ti_sbio,
			LBER_SBIOD_LEVEL_TRANSPORT, (void *)ssl );
	}

	err = tls_imp->ti_session_accept( ssl );

#ifdef HAVE_WINSOCK
	errno = WSAGetLastError();
#endif

	if ( err < 0 )
	{
		if ( update_flags( sb, ssl, err )) return 1;

		if ( DebugTest( LDAP_DEBUG_ANY ) ) {
			char buf[256], *msg;
			msg = tls_imp->ti_session_errmsg( ssl, err, buf, sizeof(buf) );
			Debug( LDAP_DEBUG_ANY,"TLS: can't accept: %s.\n",
				msg ? msg : "(unknown)", 0, 0 );
		}

		ber_sockbuf_remove_io( sb, tls_imp->ti_sbio,
			LBER_SBIOD_LEVEL_TRANSPORT );
#ifdef LDAP_DEBUG
		ber_sockbuf_remove_io( sb, &ber_sockbuf_io_debug,
			LBER_SBIOD_LEVEL_TRANSPORT );
#endif
		return -1;
	}
	return 0;
}