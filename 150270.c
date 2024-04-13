static ssize_t ucma_set_option(struct ucma_file *file, const char __user *inbuf,
			       int in_len, int out_len)
{
	struct rdma_ucm_set_option cmd;
	struct ucma_context *ctx;
	void *optval;
	int ret;

	if (copy_from_user(&cmd, inbuf, sizeof(cmd)))
		return -EFAULT;

	if (unlikely(cmd.optlen > KMALLOC_MAX_SIZE))
		return -EINVAL;

	ctx = ucma_get_ctx(file, cmd.id);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	optval = memdup_user(u64_to_user_ptr(cmd.optval),
			     cmd.optlen);
	if (IS_ERR(optval)) {
		ret = PTR_ERR(optval);
		goto out;
	}

	ret = ucma_set_option_level(ctx, cmd.level, cmd.optname, optval,
				    cmd.optlen);
	kfree(optval);

out:
	ucma_put_ctx(ctx);
	return ret;
}