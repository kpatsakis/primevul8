parseReadAttrs(
	Operation *op,
	SlapReply *rs,
	LDAPControl *ctrl,
	int post )
{
	ber_len_t	siz, off, i;
	BerElement	*ber;
	AttributeName	*an = NULL;

	if ( ( post && op->o_postread != SLAP_CONTROL_NONE ) ||
		( !post && op->o_preread != SLAP_CONTROL_NONE ) )
	{
		rs->sr_text = READMSG( post, "specified multiple times" );
		return LDAP_PROTOCOL_ERROR;
	}

	if ( BER_BVISNULL( &ctrl->ldctl_value ) ) {
		rs->sr_text = READMSG( post, "value is absent" );
		return LDAP_PROTOCOL_ERROR;
	}

	if ( BER_BVISEMPTY( &ctrl->ldctl_value ) ) {
		rs->sr_text = READMSG( post, "value is empty" );
		return LDAP_PROTOCOL_ERROR;
	}

#ifdef LDAP_X_TXN
	if ( op->o_txnSpec ) { /* temporary limitation */
		rs->sr_text = READMSG( post, "cannot perform in transaction" );
		return LDAP_UNWILLING_TO_PERFORM;
	}
#endif

	ber = ber_init( &ctrl->ldctl_value );
	if ( ber == NULL ) {
		rs->sr_text = READMSG( post, "internal error" );
		return LDAP_OTHER;
	}

	rs->sr_err = LDAP_SUCCESS;
	siz = sizeof( AttributeName );
	off = offsetof( AttributeName, an_name );
	if ( ber_scanf( ber, "{M}", &an, &siz, off ) == LBER_ERROR ) {
		rs->sr_text = READMSG( post, "decoding error" );
		rs->sr_err = LDAP_PROTOCOL_ERROR;
		goto done;
	}

	for ( i = 0; i < siz; i++ ) {
		const char	*dummy = NULL;
		int		rc;

		an[i].an_desc = NULL;
		an[i].an_oc = NULL;
		an[i].an_flags = 0;
		rc = slap_bv2ad( &an[i].an_name, &an[i].an_desc, &dummy );
		if ( rc == LDAP_SUCCESS ) {
			an[i].an_name = an[i].an_desc->ad_cname;

		} else {
			int			j;
			static struct berval	special_attrs[] = {
				BER_BVC( LDAP_NO_ATTRS ),
				BER_BVC( LDAP_ALL_USER_ATTRIBUTES ),
				BER_BVC( LDAP_ALL_OPERATIONAL_ATTRIBUTES ),
				BER_BVNULL
			};

			/* deal with special attribute types */
			for ( j = 0; !BER_BVISNULL( &special_attrs[ j ] ); j++ ) {
				if ( bvmatch( &an[i].an_name, &special_attrs[ j ] ) ) {
					an[i].an_name = special_attrs[ j ];
					break;
				}
			}

			if ( BER_BVISNULL( &special_attrs[ j ] ) && ctrl->ldctl_iscritical ) {
				rs->sr_err = rc;
				rs->sr_text = dummy ? dummy
					: READMSG( post, "unknown attributeType" );
				goto done;
			}
		}
	}

	if ( post ) {
		op->o_postread_attrs = an;
		op->o_postread = ctrl->ldctl_iscritical
			? SLAP_CONTROL_CRITICAL
			: SLAP_CONTROL_NONCRITICAL;
	} else {
		op->o_preread_attrs = an;
		op->o_preread = ctrl->ldctl_iscritical
			? SLAP_CONTROL_CRITICAL
			: SLAP_CONTROL_NONCRITICAL;
	}

done:
	(void) ber_free( ber, 1 );
	return rs->sr_err;
}