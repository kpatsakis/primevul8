
static __cold void io_uring_show_fdinfo(struct seq_file *m, struct file *f)
{
	struct io_ring_ctx *ctx = f->private_data;

	if (percpu_ref_tryget(&ctx->refs)) {
		__io_uring_show_fdinfo(ctx, m);
		percpu_ref_put(&ctx->refs);
	}