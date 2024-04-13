bitStringValidate(
	Syntax *syntax,
	struct berval *in )
{
	ber_len_t i;

	/* very unforgiving validation, requires no normalization
	 * before simplistic matching
	 */
	if( in->bv_len < 3 ) {
		return LDAP_INVALID_SYNTAX;
	}

	/* RFC 4517 Section 3.3.2 Bit String:
	 *	BitString    = SQUOTE *binary-digit SQUOTE "B"
	 *	binary-digit = "0" / "1"
	 *
	 * where SQUOTE [RFC4512] is
	 *	SQUOTE  = %x27 ; single quote ("'")
	 *
	 * Example: '0101111101'B
	 */
	
	if( in->bv_val[0] != '\'' ||
		in->bv_val[in->bv_len - 2] != '\'' ||
		in->bv_val[in->bv_len - 1] != 'B' )
	{
		return LDAP_INVALID_SYNTAX;
	}

	for( i = in->bv_len - 3; i > 0; i-- ) {
		if( in->bv_val[i] != '0' && in->bv_val[i] != '1' ) {
			return LDAP_INVALID_SYNTAX;
		}
	}

	return LDAP_SUCCESS;
}