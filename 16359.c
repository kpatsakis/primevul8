 */
static struct file *io_uring_get_file(struct io_ring_ctx *ctx)
{
	struct file *file;
#if defined(CONFIG_UNIX)
	int ret;

	ret = sock_create_kern(&init_net, PF_UNIX, SOCK_RAW, IPPROTO_IP,
				&ctx->ring_sock);
	if (ret)
		return ERR_PTR(ret);
#endif

	file = anon_inode_getfile_secure("[io_uring]", &io_uring_fops, ctx,
					 O_RDWR | O_CLOEXEC, NULL);
#if defined(CONFIG_UNIX)
	if (IS_ERR(file)) {
		sock_release(ctx->ring_sock);
		ctx->ring_sock = NULL;
	} else {
		ctx->ring_sock->file = file;
	}
#endif
	return file;