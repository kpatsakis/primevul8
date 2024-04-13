fz_drop_link_key(fz_context *ctx, void *key_)
{
	fz_link_key *key = (fz_link_key *)key_;
	if (fz_drop_imp(ctx, key, &key->refs))
		fz_free(ctx, key);
}