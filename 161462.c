static inline void debug_work_activate(struct work_struct *work)
{
	debug_object_activate(work, &work_debug_descr);
}