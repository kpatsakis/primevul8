slap_controls_init( void )
{
	int i, rc;

	rc = LDAP_SUCCESS;

	for ( i = 0; control_defs[i].sc_oid != NULL; i++ ) {
		int *cid = (int *)(((char *)&slap_cids) + control_defs[i].sc_cid );
		rc = register_supported_control( control_defs[i].sc_oid,
			control_defs[i].sc_mask, control_defs[i].sc_extendedops,
			control_defs[i].sc_parse, cid );
		if ( rc != LDAP_SUCCESS ) break;
	}

	return rc;
}