kickoff_adverts(void)
{
	struct Interface *iface;

	/*
	 *	send initial advertisement and set timers
	 */

	for(iface=IfaceList; iface; iface=iface->next)
	{
		double next;


		gettimeofday(&iface->last_ra_time, NULL);

		if( iface->UnicastOnly )
			continue;

		gettimeofday(&iface->last_multicast, NULL);

		if (!iface->AdvSendAdvert)
			continue;

		/* send an initial advertisement */
		if (send_ra_forall(iface, NULL) == 0) {

			iface->init_racount++;

			next = min(MAX_INITIAL_RTR_ADVERT_INTERVAL, iface->MaxRtrAdvInterval);
			iface->next_multicast = next_timeval(next);
		}
	}
}