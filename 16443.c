	__must_hold(&ctx->completion_lock)
	__must_hold(&ctx->timeout_lock)
{
	struct io_kiocb *req = io_timeout_extract(ctx, user_data);

	if (IS_ERR(req))
		return PTR_ERR(req);
	io_req_task_queue_fail(req, -ECANCELED);
	return 0;