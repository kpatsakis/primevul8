slap_schema_init( void )
{
	int		res;
	int		i;

	/* we should only be called once (from main) */
	assert( schema_init_done == 0 );

	for ( i=0; syntax_defs[i].sd_desc != NULL; i++ ) {
		res = register_syntax( &syntax_defs[i] );

		if ( res ) {
			fprintf( stderr, "slap_schema_init: Error registering syntax %s\n",
				 syntax_defs[i].sd_desc );
			return LDAP_OTHER;
		}
	}

	for ( i=0; mrule_defs[i].mrd_desc != NULL; i++ ) {
		if( mrule_defs[i].mrd_usage == SLAP_MR_NONE &&
			mrule_defs[i].mrd_compat_syntaxes == NULL )
		{
			fprintf( stderr,
				"slap_schema_init: Ignoring unusable matching rule %s\n",
				 mrule_defs[i].mrd_desc );
			continue;
		}

		res = register_matching_rule( &mrule_defs[i] );

		if ( res ) {
			fprintf( stderr,
				"slap_schema_init: Error registering matching rule %s\n",
				 mrule_defs[i].mrd_desc );
			return LDAP_OTHER;
		}
	}

	res = slap_schema_load();
	schema_init_done = 1;
	return res;
}