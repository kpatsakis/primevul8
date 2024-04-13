bool kthread_cancel_delayed_work_sync(struct kthread_delayed_work *dwork)
{
	return __kthread_cancel_work_sync(&dwork->work, true);
}