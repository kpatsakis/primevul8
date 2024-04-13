void __init_work(struct work_struct *work, int onstack)
{
	if (onstack)
		debug_object_init_on_stack(work, &work_debug_descr);
	else
		debug_object_init(work, &work_debug_descr);
}