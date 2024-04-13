UTF8StringValidate(
	Syntax *syntax,
	struct berval *in )
{
	int len;
	unsigned char *u = (unsigned char *)in->bv_val, *end = in->bv_val + in->bv_len;

	if( BER_BVISEMPTY( in ) && syntax == slap_schema.si_syn_directoryString ) {
		/* directory strings cannot be empty */
		return LDAP_INVALID_SYNTAX;
	}

	for( ; u < end; u += len ) {
		/* get the length indicated by the first byte */
		len = LDAP_UTF8_CHARLEN2( u, len );

		/* very basic checks */
		switch( len ) {
			case 6:
				if( (u[5] & 0xC0) != 0x80 ) {
					return LDAP_INVALID_SYNTAX;
				}
			case 5:
				if( (u[4] & 0xC0) != 0x80 ) {
					return LDAP_INVALID_SYNTAX;
				}
			case 4:
				if( (u[3] & 0xC0) != 0x80 ) {
					return LDAP_INVALID_SYNTAX;
				}
			case 3:
				if( (u[2] & 0xC0 )!= 0x80 ) {
					return LDAP_INVALID_SYNTAX;
				}
			case 2:
				if( (u[1] & 0xC0) != 0x80 ) {
					return LDAP_INVALID_SYNTAX;
				}
			case 1:
				/* CHARLEN already validated it */
				break;
			default:
				return LDAP_INVALID_SYNTAX;
		}

		/* make sure len corresponds with the offset
			to the next character */
		if( LDAP_UTF8_OFFSET( (char *)u ) != len ) return LDAP_INVALID_SYNTAX;
	}

	if( u > end ) {
		return LDAP_INVALID_SYNTAX;
	}

	return LDAP_SUCCESS;
}