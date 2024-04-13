unregister_supported_control( const char *controloid )
{
	struct slap_control *sc;
	int i;

	if ( controloid == NULL || (sc = find_ctrl( controloid )) == NULL ){
		return -1;
	}

	for ( i = 0; slap_known_controls[ i ]; i++ ) {
		if ( strcmp( controloid, slap_known_controls[ i ] ) == 0 ) {
			do {
				slap_known_controls[ i ] = slap_known_controls[ i+1 ];
			} while ( slap_known_controls[ i++ ] );
			num_known_controls--;
			break;
		}
	}

	LDAP_SLIST_REMOVE(&controls_list, sc, slap_control, sc_next);
	ch_free( sc->sc_oid );
	if ( sc->sc_extendedopsbv != NULL ) {
		ber_bvarray_free( sc->sc_extendedopsbv );
	}
	ch_free( sc );

	return 0;
}