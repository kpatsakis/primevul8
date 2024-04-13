slap_ctrl_session_tracking_request_add( Operation *op, SlapReply *rs, LDAPControl *ctrl )
{
	static struct berval	bv_unknown = BER_BVC( SLAP_STRING_UNKNOWN );
	struct berval		ip = BER_BVNULL,
				name = BER_BVNULL,
				id = BER_BVNULL;

	if ( !BER_BVISNULL( &op->o_conn->c_peer_name ) &&
		memcmp( op->o_conn->c_peer_name.bv_val, "IP=", STRLENOF( "IP=" ) ) == 0 )
	{
		char	*ptr;

		ip.bv_val = op->o_conn->c_peer_name.bv_val + STRLENOF( "IP=" );
		ip.bv_len = op->o_conn->c_peer_name.bv_len - STRLENOF( "IP=" );

		ptr = ber_bvchr( &ip, ':' );
		if ( ptr ) {
			ip.bv_len = ptr - ip.bv_val;
		}
	}

	if ( !BER_BVISNULL( &op->o_conn->c_peer_domain ) &&
		!bvmatch( &op->o_conn->c_peer_domain, &bv_unknown ) )
	{
		name = op->o_conn->c_peer_domain;
	}

	if ( !BER_BVISNULL( &op->o_dn ) && !BER_BVISEMPTY( &op->o_dn ) ) {
		id = op->o_dn;
	}

	return slap_ctrl_session_tracking_add( op, rs, &ip, &name, &id, ctrl );
}