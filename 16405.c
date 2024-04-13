
void __io_uring_cancel(bool cancel_all)
{
	io_uring_cancel_generic(cancel_all, NULL);