ldap_pvt_tls_sb_ctx( Sockbuf *sb )
{
	void			*p = NULL;
	
	ber_sockbuf_ctrl( sb, LBER_SB_OPT_GET_SSL, (void *)&p );
	return p;
}