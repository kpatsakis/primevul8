static void *timer_debug_hint(void *addr)
{
	return ((struct timer_list *) addr)->function;
}