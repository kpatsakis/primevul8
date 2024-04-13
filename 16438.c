		       const struct io_uring_sqe *sqe)
	__must_hold(&ctx->uring_lock)
{
	unsigned int sqe_flags;
	int personality;
	u8 opcode;

	/* req is partially pre-initialised, see io_preinit_req() */
	req->opcode = opcode = READ_ONCE(sqe->opcode);
	/* same numerical values with corresponding REQ_F_*, safe to copy */
	req->flags = sqe_flags = READ_ONCE(sqe->flags);
	req->user_data = READ_ONCE(sqe->user_data);
	req->file = NULL;
	req->fixed_rsrc_refs = NULL;
	req->task = current;

	if (unlikely(opcode >= IORING_OP_LAST)) {
		req->opcode = 0;
		return -EINVAL;
	}
	if (unlikely(sqe_flags & ~SQE_COMMON_FLAGS)) {
		/* enforce forwards compatibility on users */
		if (sqe_flags & ~SQE_VALID_FLAGS)
			return -EINVAL;
		if ((sqe_flags & IOSQE_BUFFER_SELECT) &&
		    !io_op_defs[opcode].buffer_select)
			return -EOPNOTSUPP;
		if (sqe_flags & IOSQE_CQE_SKIP_SUCCESS)
			ctx->drain_disabled = true;
		if (sqe_flags & IOSQE_IO_DRAIN) {
			if (ctx->drain_disabled)
				return -EOPNOTSUPP;
			io_init_req_drain(req);
		}
	}
	if (unlikely(ctx->restricted || ctx->drain_active || ctx->drain_next)) {
		if (ctx->restricted && !io_check_restriction(ctx, req, sqe_flags))
			return -EACCES;
		/* knock it to the slow queue path, will be drained there */
		if (ctx->drain_active)
			req->flags |= REQ_F_FORCE_ASYNC;
		/* if there is no link, we're at "next" request and need to drain */
		if (unlikely(ctx->drain_next) && !ctx->submit_state.link.head) {
			ctx->drain_next = false;
			ctx->drain_active = true;
			req->flags |= REQ_F_IO_DRAIN | REQ_F_FORCE_ASYNC;
		}
	}

	if (io_op_defs[opcode].needs_file) {
		struct io_submit_state *state = &ctx->submit_state;

		req->fd = READ_ONCE(sqe->fd);

		/*
		 * Plug now if we have more than 2 IO left after this, and the
		 * target is potentially a read/write to block based storage.
		 */
		if (state->need_plug && io_op_defs[opcode].plug) {
			state->plug_started = true;
			state->need_plug = false;
			blk_start_plug_nr_ios(&state->plug, state->submit_nr);
		}
	}

	personality = READ_ONCE(sqe->personality);
	if (personality) {
		int ret;

		req->creds = xa_load(&ctx->personalities, personality);
		if (!req->creds)
			return -EINVAL;
		get_cred(req->creds);
		ret = security_uring_override_creds(req->creds);
		if (ret) {
			put_cred(req->creds);
			return ret;
		}
		req->flags |= REQ_F_CREDS;
	}

	return io_req_prep(req, sqe);