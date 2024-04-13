bool kthread_cancel_work_sync(struct kthread_work *work)
{
	return __kthread_cancel_work_sync(work, false);
}