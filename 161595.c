void destroy_work_on_stack(struct work_struct *work)
{
	debug_object_free(work, &work_debug_descr);
}