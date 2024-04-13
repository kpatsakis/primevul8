	__releases(ctx->uring_lock)
	__acquires(ctx->uring_lock)
{
	int ret;

	/*
	 * We're inside the ring mutex, if the ref is already dying, then
	 * someone else killed the ctx or is already going through
	 * io_uring_register().
	 */
	if (percpu_ref_is_dying(&ctx->refs))
		return -ENXIO;

	if (ctx->restricted) {
		if (opcode >= IORING_REGISTER_LAST)
			return -EINVAL;
		opcode = array_index_nospec(opcode, IORING_REGISTER_LAST);
		if (!test_bit(opcode, ctx->restrictions.register_op))
			return -EACCES;
	}

	switch (opcode) {
	case IORING_REGISTER_BUFFERS:
		ret = io_sqe_buffers_register(ctx, arg, nr_args, NULL);
		break;
	case IORING_UNREGISTER_BUFFERS:
		ret = -EINVAL;
		if (arg || nr_args)
			break;
		ret = io_sqe_buffers_unregister(ctx);
		break;
	case IORING_REGISTER_FILES:
		ret = io_sqe_files_register(ctx, arg, nr_args, NULL);
		break;
	case IORING_UNREGISTER_FILES:
		ret = -EINVAL;
		if (arg || nr_args)
			break;
		ret = io_sqe_files_unregister(ctx);
		break;
	case IORING_REGISTER_FILES_UPDATE:
		ret = io_register_files_update(ctx, arg, nr_args);
		break;
	case IORING_REGISTER_EVENTFD:
		ret = -EINVAL;
		if (nr_args != 1)
			break;
		ret = io_eventfd_register(ctx, arg, 0);
		break;
	case IORING_REGISTER_EVENTFD_ASYNC:
		ret = -EINVAL;
		if (nr_args != 1)
			break;
		ret = io_eventfd_register(ctx, arg, 1);
		break;
	case IORING_UNREGISTER_EVENTFD:
		ret = -EINVAL;
		if (arg || nr_args)
			break;
		ret = io_eventfd_unregister(ctx);
		break;
	case IORING_REGISTER_PROBE:
		ret = -EINVAL;
		if (!arg || nr_args > 256)
			break;
		ret = io_probe(ctx, arg, nr_args);
		break;
	case IORING_REGISTER_PERSONALITY:
		ret = -EINVAL;
		if (arg || nr_args)
			break;
		ret = io_register_personality(ctx);
		break;
	case IORING_UNREGISTER_PERSONALITY:
		ret = -EINVAL;
		if (arg)
			break;
		ret = io_unregister_personality(ctx, nr_args);
		break;
	case IORING_REGISTER_ENABLE_RINGS:
		ret = -EINVAL;
		if (arg || nr_args)
			break;
		ret = io_register_enable_rings(ctx);
		break;
	case IORING_REGISTER_RESTRICTIONS:
		ret = io_register_restrictions(ctx, arg, nr_args);
		break;
	case IORING_REGISTER_FILES2:
		ret = io_register_rsrc(ctx, arg, nr_args, IORING_RSRC_FILE);
		break;
	case IORING_REGISTER_FILES_UPDATE2:
		ret = io_register_rsrc_update(ctx, arg, nr_args,
					      IORING_RSRC_FILE);
		break;
	case IORING_REGISTER_BUFFERS2:
		ret = io_register_rsrc(ctx, arg, nr_args, IORING_RSRC_BUFFER);
		break;
	case IORING_REGISTER_BUFFERS_UPDATE:
		ret = io_register_rsrc_update(ctx, arg, nr_args,
					      IORING_RSRC_BUFFER);
		break;
	case IORING_REGISTER_IOWQ_AFF:
		ret = -EINVAL;
		if (!arg || !nr_args)
			break;
		ret = io_register_iowq_aff(ctx, arg, nr_args);
		break;
	case IORING_UNREGISTER_IOWQ_AFF:
		ret = -EINVAL;
		if (arg || nr_args)
			break;
		ret = io_unregister_iowq_aff(ctx);
		break;
	case IORING_REGISTER_IOWQ_MAX_WORKERS:
		ret = -EINVAL;
		if (!arg || nr_args != 2)
			break;
		ret = io_register_iowq_max_workers(ctx, arg);
		break;
	case IORING_REGISTER_RING_FDS:
		ret = io_ringfd_register(ctx, arg, nr_args);
		break;
	case IORING_UNREGISTER_RING_FDS:
		ret = io_ringfd_unregister(ctx, arg, nr_args);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;