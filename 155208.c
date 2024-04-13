ldap_int_tls_start ( LDAP *ld, LDAPConn *conn, LDAPURLDesc *srv )
{
	Sockbuf *sb;
	char *host;
	void *ssl;
	int ret, async;
#ifdef LDAP_USE_NON_BLOCKING_TLS
	struct timeval start_time_tv, tv, tv0;
	ber_socket_t	sd = AC_SOCKET_ERROR;
#endif /* LDAP_USE_NON_BLOCKING_TLS */

	if ( !conn )
		return LDAP_PARAM_ERROR;

	sb = conn->lconn_sb;
	if( srv ) {
		host = srv->lud_host;
	} else {
 		host = conn->lconn_server->lud_host;
	}

	/* avoid NULL host */
	if( host == NULL ) {
		host = "localhost";
	}

	(void) tls_init( tls_imp );

#ifdef LDAP_USE_NON_BLOCKING_TLS
	/*
	 * Use non-blocking io during SSL Handshake when a timeout is configured
	 */
	async = LDAP_BOOL_GET( &ld->ld_options, LDAP_BOOL_CONNECT_ASYNC );
	if ( ld->ld_options.ldo_tm_net.tv_sec >= 0 ) {
		if ( !async ) {
			/* if async, this has already been set */
			ber_sockbuf_ctrl( sb, LBER_SB_OPT_SET_NONBLOCK, (void*)1 );
		}
		ber_sockbuf_ctrl( sb, LBER_SB_OPT_GET_FD, &sd );
		tv = ld->ld_options.ldo_tm_net;
		tv0 = tv;
#ifdef HAVE_GETTIMEOFDAY
		gettimeofday( &start_time_tv, NULL );
#else /* ! HAVE_GETTIMEOFDAY */
		time( &start_time_tv.tv_sec );
		start_time_tv.tv_usec = 0;
#endif /* ! HAVE_GETTIMEOFDAY */
	}

#endif /* LDAP_USE_NON_BLOCKING_TLS */

	ld->ld_errno = LDAP_SUCCESS;
	ret = ldap_int_tls_connect( ld, conn, host );

	 /* this mainly only happens for non-blocking io
	  * but can also happen when the handshake is too
	  * big for a single network message.
	  */
	while ( ret > 0 ) {
#ifdef LDAP_USE_NON_BLOCKING_TLS
		if ( async ) {
			struct timeval curr_time_tv, delta_tv;
			int wr=0;

			if ( sb->sb_trans_needs_read ) {
				wr=0;
			} else if ( sb->sb_trans_needs_write ) {
				wr=1;
			}
			Debug( LDAP_DEBUG_TRACE, "ldap_int_tls_start: ldap_int_tls_connect needs %s\n",
					wr ? "write": "read", 0, 0 );

			/* This is mostly copied from result.c:wait4msg(), should
			 * probably be moved into a separate function */
#ifdef HAVE_GETTIMEOFDAY
			gettimeofday( &curr_time_tv, NULL );
#else /* ! HAVE_GETTIMEOFDAY */
			time( &curr_time_tv.tv_sec );
			curr_time_tv.tv_usec = 0;
#endif /* ! HAVE_GETTIMEOFDAY */

			/* delta = curr - start */
			delta_tv.tv_sec = curr_time_tv.tv_sec - start_time_tv.tv_sec;
			delta_tv.tv_usec = curr_time_tv.tv_usec - start_time_tv.tv_usec;
			if ( delta_tv.tv_usec < 0 ) {
				delta_tv.tv_sec--;
				delta_tv.tv_usec += 1000000;
			}

			/* tv0 < delta ? */
			if ( ( tv0.tv_sec < delta_tv.tv_sec ) ||
				 ( ( tv0.tv_sec == delta_tv.tv_sec ) &&
				   ( tv0.tv_usec < delta_tv.tv_usec ) ) )
			{
				ret = -1;
				ld->ld_errno = LDAP_TIMEOUT;
				break;
			}
			/* timeout -= delta_time */
			tv0.tv_sec -= delta_tv.tv_sec;
			tv0.tv_usec -= delta_tv.tv_usec;
			if ( tv0.tv_usec < 0 ) {
				tv0.tv_sec--;
				tv0.tv_usec += 1000000;
			}
			start_time_tv.tv_sec = curr_time_tv.tv_sec;
			start_time_tv.tv_usec = curr_time_tv.tv_usec;
			tv = tv0;
			Debug( LDAP_DEBUG_TRACE, "ldap_int_tls_start: ld %p %ld s %ld us to go\n",
				(void *)ld, (long) tv.tv_sec, (long) tv.tv_usec );
			ret = ldap_int_poll( ld, sd, &tv, wr);
			if ( ret < 0 ) {
				ld->ld_errno = LDAP_TIMEOUT;
				break;
			}
		}
#endif /* LDAP_USE_NON_BLOCKING_TLS */
		ret = ldap_int_tls_connect( ld, conn, host );
	}

	if ( ret < 0 ) {
		if ( ld->ld_errno == LDAP_SUCCESS )
			ld->ld_errno = LDAP_CONNECT_ERROR;
		return (ld->ld_errno);
	}

	return LDAP_SUCCESS;
}