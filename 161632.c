static void set_work_pwq(struct work_struct *work, struct pool_workqueue *pwq,
			 unsigned long extra_flags)
{
	set_work_data(work, (unsigned long)pwq,
		      WORK_STRUCT_PENDING | WORK_STRUCT_PWQ | extra_flags);
}