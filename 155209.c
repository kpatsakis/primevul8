update_flags( Sockbuf *sb, tls_session * ssl, int rc )
{
	sb->sb_trans_needs_read  = 0;
	sb->sb_trans_needs_write = 0;

	return tls_imp->ti_session_upflags( sb, ssl, rc );
}