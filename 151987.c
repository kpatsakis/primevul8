find_ctrl( const char *oid )
{
	struct slap_control *sc;

	LDAP_SLIST_FOREACH( sc, &controls_list, sc_next ) {
		if ( strcmp( oid, sc->sc_oid ) == 0 ) {
			return sc;
		}
	}

	return NULL;
}