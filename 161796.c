static void *work_debug_hint(void *addr)
{
	return ((struct work_struct *) addr)->func;
}