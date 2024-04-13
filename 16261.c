				    struct timespec64 *ts, enum hrtimer_mode mode)
	__must_hold(&ctx->timeout_lock)
{
	struct io_timeout_data *io;
	struct io_kiocb *req;
	bool found = false;

	list_for_each_entry(req, &ctx->ltimeout_list, timeout.list) {
		found = user_data == req->user_data;
		if (found)
			break;
	}
	if (!found)
		return -ENOENT;

	io = req->async_data;
	if (hrtimer_try_to_cancel(&io->timer) == -1)
		return -EALREADY;
	hrtimer_init(&io->timer, io_timeout_get_clock(io), mode);
	io->timer.function = io_link_timeout_fn;
	hrtimer_start(&io->timer, timespec64_to_ktime(*ts), mode);
	return 0;