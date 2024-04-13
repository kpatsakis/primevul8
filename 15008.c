sidNormalize(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *normalized,
	void *ctx )
{
	if ( val->bv_len != 3 ) {
		return LDAP_INVALID_SYNTAX;
	}

	return hexNormalize( 0, NULL, NULL, val, normalized, ctx );
}