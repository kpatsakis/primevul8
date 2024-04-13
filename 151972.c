controls_root_dse_info( Entry *e )
{
	AttributeDescription *ad_supportedControl
		= slap_schema.si_ad_supportedControl;
	struct berval vals[2];
	struct slap_control *sc;

	vals[1].bv_val = NULL;
	vals[1].bv_len = 0;

	LDAP_SLIST_FOREACH( sc, &controls_list, sc_next ) {
		if( sc->sc_mask & SLAP_CTRL_HIDE ) continue;

		vals[0].bv_val = sc->sc_oid;
		vals[0].bv_len = strlen( sc->sc_oid );

		if ( attr_merge( e, ad_supportedControl, vals, NULL ) ) {
			return -1;
		}
	}

	return 0;
}