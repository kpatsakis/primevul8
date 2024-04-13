ldns_fget_token(FILE *f, char *token, const char *delim, size_t limit)
{
	return ldns_fget_token_l(f, token, delim, limit, NULL);
}