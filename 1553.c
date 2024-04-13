static void __nfs42_ssc_close(struct file *filep)
{
	struct nfs_open_context *ctx = nfs_file_open_context(filep);

	ctx->state->flags = 0;
}