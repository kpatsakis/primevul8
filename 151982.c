controls_destroy( void )
{
	struct slap_control *sc;

	while ( !LDAP_SLIST_EMPTY(&controls_list) ) {
		sc = LDAP_SLIST_FIRST(&controls_list);
		LDAP_SLIST_REMOVE_HEAD(&controls_list, sc_next);

		ch_free( sc->sc_oid );
		if ( sc->sc_extendedopsbv != NULL ) {
			ber_bvarray_free( sc->sc_extendedopsbv );
		}
		ch_free( sc );
	}
}