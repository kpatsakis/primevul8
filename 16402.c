
static __cold int io_probe(struct io_ring_ctx *ctx, void __user *arg,
			   unsigned nr_args)
{
	struct io_uring_probe *p;
	size_t size;
	int i, ret;

	size = struct_size(p, ops, nr_args);
	if (size == SIZE_MAX)
		return -EOVERFLOW;
	p = kzalloc(size, GFP_KERNEL);
	if (!p)
		return -ENOMEM;

	ret = -EFAULT;
	if (copy_from_user(p, arg, size))
		goto out;
	ret = -EINVAL;
	if (memchr_inv(p, 0, size))
		goto out;

	p->last_op = IORING_OP_LAST - 1;
	if (nr_args > IORING_OP_LAST)
		nr_args = IORING_OP_LAST;

	for (i = 0; i < nr_args; i++) {
		p->ops[i].op = i;
		if (!io_op_defs[i].not_supported)
			p->ops[i].flags = IO_URING_OP_SUPPORTED;
	}
	p->ops_len = i;

	ret = 0;
	if (copy_to_user(arg, p, size))
		ret = -EFAULT;
out:
	kfree(p);
	return ret;