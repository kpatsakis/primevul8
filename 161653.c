static bool timer_is_static_object(void *addr)
{
	struct timer_list *timer = addr;

	return (timer->entry.pprev == NULL &&
		timer->entry.next == TIMER_ENTRY_STATIC);
}