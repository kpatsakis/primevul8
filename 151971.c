slap_remove_control(
	Operation	*op,
	SlapReply	*rs,
	int		ctrl,
	BI_chk_controls	fnc )
{
	int		i, j;

	switch ( op->o_ctrlflag[ ctrl ] ) {
	case SLAP_CONTROL_NONCRITICAL:
		for ( i = 0, j = -1; op->o_ctrls[ i ] != NULL; i++ ) {
			if ( strcmp( op->o_ctrls[ i ]->ldctl_oid,
				slap_known_controls[ ctrl - 1 ] ) == 0 )
			{
				j = i;
			}
		}

		if ( j == -1 ) {
			rs->sr_err = LDAP_OTHER;
			break;
		}

		if ( fnc ) {
			(void)fnc( op, rs );
		}

		op->o_tmpfree( op->o_ctrls[ j ], op->o_tmpmemctx );

		if ( i > 1 ) {
			AC_MEMCPY( &op->o_ctrls[ j ], &op->o_ctrls[ j + 1 ],
				( i - j ) * sizeof( LDAPControl * ) );

		} else {
			op->o_tmpfree( op->o_ctrls, op->o_tmpmemctx );
			op->o_ctrls = NULL;
		}

		op->o_ctrlflag[ ctrl ] = SLAP_CONTROL_IGNORED;

		Debug( LDAP_DEBUG_ANY, "%s: "
			"non-critical control \"%s\" not supported; stripped.\n",
			op->o_log_prefix, slap_known_controls[ ctrl ], 0 );
		/* fall thru */

	case SLAP_CONTROL_IGNORED:
	case SLAP_CONTROL_NONE:
		rs->sr_err = SLAP_CB_CONTINUE;
		break;

	case SLAP_CONTROL_CRITICAL:
		rs->sr_err = LDAP_UNAVAILABLE_CRITICAL_EXTENSION;
		if ( fnc ) {
			(void)fnc( op, rs );
		}
		Debug( LDAP_DEBUG_ANY, "%s: "
			"critical control \"%s\" not supported.\n",
			op->o_log_prefix, slap_known_controls[ ctrl ], 0 );
		break;

	default:
		/* handle all cases! */
		assert( 0 );
	}

	return rs->sr_err;
}