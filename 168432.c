fz_cmp_link_key(fz_context *ctx, void *k0_, void *k1_)
{
	fz_link_key *k0 = (fz_link_key *)k0_;
	fz_link_key *k1 = (fz_link_key *)k1_;
	return k0->proof == k1->proof &&
		k0->src_extras == k1->src_extras &&
		k0->dst_extras == k1->dst_extras &&
		k0->copy_spots == k1->copy_spots &&
		k0->depth == k1->depth &&
		k0->rend.bp == k1->rend.bp &&
		k0->rend.ri == k1->rend.ri &&
		memcmp(k0->dst_md5, k1->dst_md5, 16) == 0 &&
		memcmp(k0->src_md5, k1->src_md5, 16);
}