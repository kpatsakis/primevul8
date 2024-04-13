static bool timer_fixup_assert_init(void *addr, enum debug_obj_state state)
{
	struct timer_list *timer = addr;

	switch (state) {
	case ODEBUG_STATE_NOTAVAILABLE:
		setup_timer(timer, stub_timer, 0);
		return true;
	default:
		return false;
	}
}