slap_find_control_id(
	const char *oid,
	int *cid )
{
	struct slap_control *ctrl = find_ctrl( oid );
	if ( ctrl ) {
		if ( cid ) *cid = ctrl->sc_cid;
		return LDAP_SUCCESS;
	}
	return LDAP_CONTROL_NOT_FOUND;
}