fz_make_hash_link_key(fz_context *ctx, fz_store_hash *hash, void *key_)
{
	fz_link_key *key = (fz_link_key *)key_;
	memcpy(hash->u.link.dst_md5, key->dst_md5, 16);
	memcpy(hash->u.link.src_md5, key->src_md5, 16);
	hash->u.link.ri = key->rend.ri;
	hash->u.link.bp = key->rend.bp;
	hash->u.link.src_extras = key->src_extras;
	hash->u.link.dst_extras = key->dst_extras;
	hash->u.link.bpp16 = key->depth == 2;
	hash->u.link.proof = key->proof;
	hash->u.link.copy_spots = key->copy_spots;
	return 1;
}