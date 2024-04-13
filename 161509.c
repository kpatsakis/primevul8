static void pwq_activate_first_delayed(struct pool_workqueue *pwq)
{
	struct work_struct *work = list_first_entry(&pwq->delayed_works,
						    struct work_struct, entry);

	pwq_activate_delayed_work(work);
}