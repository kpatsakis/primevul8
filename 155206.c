ldap_tls_inplace( LDAP *ld )
{
	Sockbuf		*sb = NULL;

	if ( ld->ld_defconn && ld->ld_defconn->lconn_sb ) {
		sb = ld->ld_defconn->lconn_sb;

	} else if ( ld->ld_sb ) {
		sb = ld->ld_sb;

	} else {
		return 0;
	}

	return ldap_pvt_tls_inplace( sb );
}