bool cancel_delayed_work(struct delayed_work *dwork)
{
	return __cancel_work(&dwork->work, true);
}