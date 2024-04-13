static bool work_fixup_init(void *addr, enum debug_obj_state state)
{
	struct work_struct *work = addr;

	switch (state) {
	case ODEBUG_STATE_ACTIVE:
		cancel_work_sync(work);
		debug_object_init(work, &work_debug_descr);
		return true;
	default:
		return false;
	}
}