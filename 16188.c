
static int io_register_personality(struct io_ring_ctx *ctx)
{
	const struct cred *creds;
	u32 id;
	int ret;

	creds = get_current_cred();

	ret = xa_alloc_cyclic(&ctx->personalities, &id, (void *)creds,
			XA_LIMIT(0, USHRT_MAX), &ctx->pers_next, GFP_KERNEL);
	if (ret < 0) {
		put_cred(creds);
		return ret;
	}
	return id;