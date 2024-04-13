
static inline void io_poll_execute(struct io_kiocb *req, int res, int events)
{
	if (io_poll_get_ownership(req))
		__io_poll_execute(req, res, events);