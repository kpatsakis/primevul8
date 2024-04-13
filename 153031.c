close_msg_fds(struct vhu_msg_context *ctx)
{
	int i;

	for (i = 0; i < ctx->fd_num; i++) {
		int fd = ctx->fds[i];

		if (fd == -1)
			continue;

		ctx->fds[i] = -1;
		close(fd);
	}
}