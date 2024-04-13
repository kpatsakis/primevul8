register_supported_control2(const char *controloid,
	slap_mask_t controlmask,
	char **controlexops,
	SLAP_CTRL_PARSE_FN *controlparsefn,
	unsigned flags,
	int *controlcid)
{
	struct slap_control *sc = NULL;
	int i;
	BerVarray extendedopsbv = NULL;

	if ( num_known_controls >= SLAP_MAX_CIDS ) {
		Debug( LDAP_DEBUG_ANY, "Too many controls registered."
			" Recompile slapd with SLAP_MAX_CIDS defined > %d\n",
		num_known_controls, 0, 0 );
		return LDAP_OTHER;
	}

	if ( controloid == NULL ) {
		return LDAP_PARAM_ERROR;
	}

	/* check if already registered */
	for ( i = 0; slap_known_controls[ i ]; i++ ) {
		if ( strcmp( controloid, slap_known_controls[ i ] ) == 0 ) {
			if ( flags == 1 ) {
				Debug( LDAP_DEBUG_TRACE,
					"Control %s already registered; replacing.\n",
					controloid, 0, 0 );
				/* (find and) replace existing handler */
				sc = find_ctrl( controloid );
				assert( sc != NULL );
				break;
			}

			Debug( LDAP_DEBUG_ANY,
				"Control %s already registered.\n",
				controloid, 0, 0 );
			return LDAP_PARAM_ERROR;
		}
	}

	/* turn compatible extended operations into bervals */
	if ( controlexops != NULL ) {
		int i;

		for ( i = 0; controlexops[ i ]; i++ );

		extendedopsbv = ber_memcalloc( i + 1, sizeof( struct berval ) );
		if ( extendedopsbv == NULL ) {
			return LDAP_NO_MEMORY;
		}

		for ( i = 0; controlexops[ i ]; i++ ) {
			ber_str2bv( controlexops[ i ], 0, 1, &extendedopsbv[ i ] );
		}
	}

	if ( sc == NULL ) {
		sc = (struct slap_control *)SLAP_MALLOC( sizeof( *sc ) );
		if ( sc == NULL ) {
			ber_bvarray_free( extendedopsbv );
			return LDAP_NO_MEMORY;
		}

		sc->sc_oid = ch_strdup( controloid );
		sc->sc_cid = num_known_controls;

		/* Update slap_known_controls, too. */
		slap_known_controls[num_known_controls - 1] = sc->sc_oid;
		slap_known_controls[num_known_controls++] = NULL;

		LDAP_SLIST_NEXT( sc, sc_next ) = NULL;
		LDAP_SLIST_INSERT_HEAD( &controls_list, sc, sc_next );

	} else {
		if ( sc->sc_extendedopsbv ) {
			/* FIXME: in principle, we should rather merge
			 * existing extops with those supported by the
			 * new control handling implementation.
			 * In fact, whether a control is compatible with
			 * an extop should not be a matter of implementation.
			 * We likely also need a means for a newly
			 * registered extop to declare that it is
			 * comptible with an already registered control.
			 */
			ber_bvarray_free( sc->sc_extendedopsbv );
			sc->sc_extendedopsbv = NULL;
			sc->sc_extendedops = NULL;
		}
	}

	sc->sc_extendedopsbv = extendedopsbv;
	sc->sc_mask = controlmask;
	sc->sc_parse = controlparsefn;
	if ( controlcid ) {
		*controlcid = sc->sc_cid;
	}

	return LDAP_SUCCESS;
}