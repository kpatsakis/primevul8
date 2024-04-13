timer_handler(void *data)
{
	struct Interface *iface = (struct Interface *) data;
	double next;

	dlog(LOG_DEBUG, 4, "timer_handler called for %s", iface->Name);

	if (send_ra_forall(iface, NULL) != 0) {
		return;
	}

	next = rand_between(iface->MinRtrAdvInterval, iface->MaxRtrAdvInterval);

	if (iface->init_racount < MAX_INITIAL_RTR_ADVERTISEMENTS)
	{
		iface->init_racount++;
		next = min(MAX_INITIAL_RTR_ADVERT_INTERVAL, next);
	}

	iface->next_multicast = next_timeval(next);
}