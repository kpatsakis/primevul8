static int cwt_wakefn(wait_queue_t *wait, unsigned mode, int sync, void *key)
{
	struct cwt_wait *cwait = container_of(wait, struct cwt_wait, wait);

	if (cwait->work != key)
		return 0;
	return autoremove_wake_function(wait, mode, sync, key);
}