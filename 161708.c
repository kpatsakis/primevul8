bool cancel_delayed_work_sync(struct delayed_work *dwork)
{
	return __cancel_work_timer(&dwork->work, true);
}