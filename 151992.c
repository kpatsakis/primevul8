slap_global_control( Operation *op, const char *oid, int *cid )
{
	struct slap_control *ctrl = find_ctrl( oid );

	if ( ctrl == NULL ) {
		/* should not be reachable */
		Debug( LDAP_DEBUG_ANY,
			"slap_global_control: unrecognized control: %s\n",      
			oid, 0, 0 );
		return LDAP_CONTROL_NOT_FOUND;
	}

	if ( cid ) *cid = ctrl->sc_cid;

	if ( ( ctrl->sc_mask & SLAP_CTRL_GLOBAL ) ||
		( ( op->o_tag & LDAP_REQ_SEARCH ) &&
		( ctrl->sc_mask & SLAP_CTRL_GLOBAL_SEARCH ) ) )
	{
		return LDAP_COMPARE_TRUE;
	}

#if 0
	Debug( LDAP_DEBUG_TRACE,
		"slap_global_control: unavailable control: %s\n",      
		oid, 0, 0 );
#endif

	return LDAP_COMPARE_FALSE;
}