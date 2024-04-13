ldap_int_tls_connect( LDAP *ld, LDAPConn *conn, const char *host )
{
	Sockbuf *sb = conn->lconn_sb;
	int	err;
	tls_session	*ssl = NULL;

	if ( HAS_TLS( sb )) {
		ber_sockbuf_ctrl( sb, LBER_SB_OPT_GET_SSL, (void *)&ssl );
	} else {
		struct ldapoptions *lo;
		tls_ctx *ctx;

		ctx = ld->ld_options.ldo_tls_ctx;

		ssl = alloc_handle( ctx, 0 );

		if ( ssl == NULL ) return -1;

#ifdef LDAP_DEBUG
		ber_sockbuf_add_io( sb, &ber_sockbuf_io_debug,
			LBER_SBIOD_LEVEL_TRANSPORT, (void *)"tls_" );
#endif
		ber_sockbuf_add_io( sb, tls_imp->ti_sbio,
			LBER_SBIOD_LEVEL_TRANSPORT, (void *)ssl );

		lo = LDAP_INT_GLOBAL_OPT();   
		if( ctx == NULL ) {
			ctx = lo->ldo_tls_ctx;
			ld->ld_options.ldo_tls_ctx = ctx;
			tls_ctx_ref( ctx );
		}
		if ( ld->ld_options.ldo_tls_connect_cb )
			ld->ld_options.ldo_tls_connect_cb( ld, ssl, ctx,
			ld->ld_options.ldo_tls_connect_arg );
		if ( lo && lo->ldo_tls_connect_cb && lo->ldo_tls_connect_cb !=
			ld->ld_options.ldo_tls_connect_cb )
			lo->ldo_tls_connect_cb( ld, ssl, ctx, lo->ldo_tls_connect_arg );
	}

	err = tls_imp->ti_session_connect( ld, ssl );

#ifdef HAVE_WINSOCK
	errno = WSAGetLastError();
#endif

	if ( err == 0 ) {
		err = ldap_pvt_tls_check_hostname( ld, ssl, host );
	}

	if ( err < 0 )
	{
		char buf[256], *msg;
		if ( update_flags( sb, ssl, err )) {
			return 1;
		}

		msg = tls_imp->ti_session_errmsg( ssl, err, buf, sizeof(buf) );
		if ( msg ) {
			if ( ld->ld_error ) {
				LDAP_FREE( ld->ld_error );
			}
			ld->ld_error = LDAP_STRDUP( msg );
#ifdef HAVE_EBCDIC
			if ( ld->ld_error ) __etoa(ld->ld_error);
#endif
		}

		Debug( LDAP_DEBUG_ANY,"TLS: can't connect: %s.\n",
			ld->ld_error ? ld->ld_error : "" ,0,0);

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