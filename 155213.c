ldap_pvt_tls_inplace ( Sockbuf *sb )
{
	return HAS_TLS( sb ) ? 1 : 0;
}