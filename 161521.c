static inline void debug_work_deactivate(struct work_struct *work)
{
	debug_object_deactivate(work, &work_debug_descr);
}