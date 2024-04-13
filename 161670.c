static void *hrtimer_debug_hint(void *addr)
{
	return ((struct hrtimer *) addr)->function;
}