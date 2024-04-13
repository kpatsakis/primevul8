slap_ctrl_whatFailed_add(
	Operation *op,
	SlapReply *rs,
	char **oids )
{
	BerElementBuffer berbuf;
	BerElement *ber = (BerElement *) &berbuf;
	LDAPControl **ctrls = NULL;
	struct berval ctrlval;
	int i, rc = LDAP_SUCCESS;

	ber_init2( ber, NULL, LBER_USE_DER );
	ber_set_option( ber, LBER_OPT_BER_MEMCTX, &op->o_tmpmemctx );
	ber_printf( ber, "[" /*]*/ );
	for ( i = 0; oids[ i ] != NULL; i++ ) {
		ber_printf( ber, "s", oids[ i ] );
	}
	ber_printf( ber, /*[*/ "]" );

	if ( ber_flatten2( ber, &ctrlval, 0 ) == -1 ) {
		rc = LDAP_OTHER;
		goto done;
	}

	i = 0;
	if ( rs->sr_ctrls != NULL ) {
		for ( ; rs->sr_ctrls[ i ] != NULL; i++ ) {
			if ( strcmp( rs->sr_ctrls[ i ]->ldctl_oid, LDAP_CONTROL_X_WHATFAILED ) != 0 ) {
				/* TODO: add */
				assert( 0 );
			}
		}
	}

	ctrls = op->o_tmprealloc( rs->sr_ctrls,
			sizeof(LDAPControl *)*( i + 2 )
			+ sizeof(LDAPControl)
			+ ctrlval.bv_len + 1,
			op->o_tmpmemctx );
	if ( ctrls == NULL ) {
		rc = LDAP_OTHER;
		goto done;
	}
	ctrls[ i + 1 ] = NULL;
	ctrls[ i ] = (LDAPControl *)&ctrls[ i + 2 ];
	ctrls[ i ]->ldctl_oid = LDAP_CONTROL_X_WHATFAILED;
	ctrls[ i ]->ldctl_iscritical = 0;
	ctrls[ i ]->ldctl_value.bv_val = (char *)&ctrls[ i ][ 1 ];
	AC_MEMCPY( ctrls[ i ]->ldctl_value.bv_val, ctrlval.bv_val, ctrlval.bv_len + 1 );
	ctrls[ i ]->ldctl_value.bv_len = ctrlval.bv_len;

	ber_free_buf( ber );

	rs->sr_ctrls = ctrls;

done:;
	return rc;
}