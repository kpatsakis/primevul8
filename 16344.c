			     struct timespec64 *ts, enum hrtimer_mode mode)
	__must_hold(&ctx->timeout_lock)
{
	struct io_kiocb *req = io_timeout_extract(ctx, user_data);
	struct io_timeout_data *data;

	if (IS_ERR(req))
		return PTR_ERR(req);

	req->timeout.off = 0; /* noseq */
	data = req->async_data;
	list_add_tail(&req->timeout.list, &ctx->timeout_list);
	hrtimer_init(&data->timer, io_timeout_get_clock(data), mode);
	data->timer.function = io_timeout_fn;
	hrtimer_start(&data->timer, timespec64_to_ktime(*ts), mode);
	return 0;