
static __cold int io_register_restrictions(struct io_ring_ctx *ctx,
					   void __user *arg, unsigned int nr_args)
{
	struct io_uring_restriction *res;
	size_t size;
	int i, ret;

	/* Restrictions allowed only if rings started disabled */
	if (!(ctx->flags & IORING_SETUP_R_DISABLED))
		return -EBADFD;

	/* We allow only a single restrictions registration */
	if (ctx->restrictions.registered)
		return -EBUSY;

	if (!arg || nr_args > IORING_MAX_RESTRICTIONS)
		return -EINVAL;

	size = array_size(nr_args, sizeof(*res));
	if (size == SIZE_MAX)
		return -EOVERFLOW;

	res = memdup_user(arg, size);
	if (IS_ERR(res))
		return PTR_ERR(res);

	ret = 0;

	for (i = 0; i < nr_args; i++) {
		switch (res[i].opcode) {
		case IORING_RESTRICTION_REGISTER_OP:
			if (res[i].register_op >= IORING_REGISTER_LAST) {
				ret = -EINVAL;
				goto out;
			}

			__set_bit(res[i].register_op,
				  ctx->restrictions.register_op);
			break;
		case IORING_RESTRICTION_SQE_OP:
			if (res[i].sqe_op >= IORING_OP_LAST) {
				ret = -EINVAL;
				goto out;
			}

			__set_bit(res[i].sqe_op, ctx->restrictions.sqe_op);
			break;
		case IORING_RESTRICTION_SQE_FLAGS_ALLOWED:
			ctx->restrictions.sqe_flags_allowed = res[i].sqe_flags;
			break;
		case IORING_RESTRICTION_SQE_FLAGS_REQUIRED:
			ctx->restrictions.sqe_flags_required = res[i].sqe_flags;
			break;
		default:
			ret = -EINVAL;
			goto out;
		}
	}

out:
	/* Reset all restrictions if an error happened */
	if (ret != 0)
		memset(&ctx->restrictions, 0, sizeof(ctx->restrictions));
	else
		ctx->restrictions.registered = true;

	kfree(res);
	return ret;