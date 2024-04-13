slap_ctrl_session_tracking_add(
	Operation *op,
	SlapReply *rs,
	struct berval *ip,
	struct berval *name,
	struct berval *id,
	LDAPControl *ctrl )
{
	BerElementBuffer berbuf;
	BerElement	*ber = (BerElement *)&berbuf;

	static struct berval	oid = BER_BVC( LDAP_CONTROL_X_SESSION_TRACKING_USERNAME );

	assert( ctrl != NULL );

	ber_init2( ber, NULL, LBER_USE_DER );

	ber_printf( ber, "{OOOO}", ip, name, &oid, id ); 

	if ( ber_flatten2( ber, &ctrl->ldctl_value, 0 ) == -1 ) {
		rs->sr_err = LDAP_OTHER;
		goto done;
	}

	ctrl->ldctl_oid = LDAP_CONTROL_X_SESSION_TRACKING;
	ctrl->ldctl_iscritical = 0;

	rs->sr_err = LDAP_SUCCESS;

done:;
	return rs->sr_err;
}