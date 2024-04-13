fz_format_link_key(fz_context *ctx, char *s, int n, void *key_)
{
	static const char *hex = "0123456789abcdef";
	fz_link_key *key = (fz_link_key *)key_;
	char sm[33], dm[33];
	int i;
	for (i = 0; i < 16; ++i)
	{
		sm[i*2+0] = hex[key->src_md5[i]>>4];
		sm[i*2+1] = hex[key->src_md5[i]&15];
		dm[i*2+0] = hex[key->dst_md5[i]>>4];
		dm[i*2+1] = hex[key->dst_md5[i]&15];
	}
	sm[32] = 0;
	dm[32] = 0;
	fz_snprintf(s, n, "(link src_md5=%s dst_md5=%s)", sm, dm);
}