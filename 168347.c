fz_keep_link_key(fz_context *ctx, void *key_)
{
	fz_link_key *key = (fz_link_key *)key_;
	return fz_keep_imp(ctx, key, &key->refs);
}