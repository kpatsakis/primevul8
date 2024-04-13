bool cancel_work(struct work_struct *work)
{
	return __cancel_work(work, false);
}