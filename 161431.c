static bool hrtimer_fixup_activate(void *addr, enum debug_obj_state state)
{
	switch (state) {
	case ODEBUG_STATE_ACTIVE:
		WARN_ON(1);

	default:
		return false;
	}
}