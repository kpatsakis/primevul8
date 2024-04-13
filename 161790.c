static bool hrtimer_fixup_free(void *addr, enum debug_obj_state state)
{
	struct hrtimer *timer = addr;

	switch (state) {
	case ODEBUG_STATE_ACTIVE:
		hrtimer_cancel(timer);
		debug_object_free(timer, &hrtimer_debug_descr);
		return true;
	default:
		return false;
	}
}