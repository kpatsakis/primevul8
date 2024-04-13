static bool timer_fixup_free(void *addr, enum debug_obj_state state)
{
	struct timer_list *timer = addr;

	switch (state) {
	case ODEBUG_STATE_ACTIVE:
		del_timer_sync(timer);
		debug_object_free(timer, &timer_debug_descr);
		return true;
	default:
		return false;
	}
}