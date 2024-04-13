UUIDPretty(
	Syntax *syntax,
	struct berval *in,
	struct berval *out,
	void *ctx )
{
	int i;
	int rc=LDAP_INVALID_SYNTAX;

	assert( in != NULL );
	assert( out != NULL );

	if( in->bv_len != 36 ) return LDAP_INVALID_SYNTAX;

	out->bv_len = 36;
	out->bv_val = slap_sl_malloc( out->bv_len + 1, ctx );

	for( i=0; i<36; i++ ) {
		switch(i) {
			case 8:
			case 13:
			case 18:
			case 23:
				if( in->bv_val[i] != '-' ) {
					goto handle_error;
				}
				out->bv_val[i] = '-';
				break;

			default:
				if( !ASCII_HEX( in->bv_val[i]) ) {
					goto handle_error;
				}
				out->bv_val[i] = TOLOWER( in->bv_val[i] );
		}
	}

	rc = LDAP_SUCCESS;
	out->bv_val[ out->bv_len ] = '\0';

	if( 0 ) {
handle_error:
		slap_sl_free( out->bv_val, ctx );
		out->bv_val = NULL;
	}

	return rc;
}