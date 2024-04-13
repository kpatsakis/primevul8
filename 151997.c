get_supported_controls(char ***ctrloidsp,
	slap_mask_t **ctrlmasks)
{
	int n;
	char **oids;
	slap_mask_t *masks;
	struct slap_control *sc;

	n = 0;

	LDAP_SLIST_FOREACH( sc, &controls_list, sc_next ) {
		n++;
	}

	if ( n == 0 ) {
		*ctrloidsp = NULL;
		*ctrlmasks = NULL;
		return LDAP_SUCCESS;
	}

	oids = (char **)SLAP_MALLOC( (n + 1) * sizeof(char *) );
	if ( oids == NULL ) {
		return LDAP_NO_MEMORY;
	}
	masks = (slap_mask_t *)SLAP_MALLOC( (n + 1) * sizeof(slap_mask_t) );
	if  ( masks == NULL ) {
		SLAP_FREE( oids );
		return LDAP_NO_MEMORY;
	}

	n = 0;

	LDAP_SLIST_FOREACH( sc, &controls_list, sc_next ) {
		oids[n] = ch_strdup( sc->sc_oid );
		masks[n] = sc->sc_mask;
		n++;
	}
	oids[n] = NULL;
	masks[n] = 0;

	*ctrloidsp = oids;
	*ctrlmasks = masks;

	return LDAP_SUCCESS;
}