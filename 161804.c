bool cancel_work_sync(struct work_struct *work)
{
	return __cancel_work_timer(work, false);
}