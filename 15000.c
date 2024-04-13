numericoidValidate(
	Syntax *syntax,
	struct berval *in )
{
	struct berval val = *in;

	if( BER_BVISEMPTY( &val ) ) {
		/* disallow empty strings */
		return LDAP_INVALID_SYNTAX;
	}

	while( OID_LEADCHAR( val.bv_val[0] ) ) {
		if ( val.bv_len == 1 ) {
			return LDAP_SUCCESS;
		}

		if ( val.bv_val[0] == '0' && !OID_SEPARATOR( val.bv_val[1] )) {
			break;
		}

		val.bv_val++;
		val.bv_len--;

		while ( OID_LEADCHAR( val.bv_val[0] )) {
			val.bv_val++;
			val.bv_len--;

			if ( val.bv_len == 0 ) {
				return LDAP_SUCCESS;
			}
		}

		if( !OID_SEPARATOR( val.bv_val[0] )) {
			break;
		}

		val.bv_val++;
		val.bv_len--;
	}

	return LDAP_INVALID_SYNTAX;
}