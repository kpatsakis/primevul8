stop_adverts(void)
{
	struct Interface *iface;

	/*
	 *	send final RA (a SHOULD in RFC4861 section 6.2.5)
	 */

	for (iface=IfaceList; iface; iface=iface->next) {
		if( ! iface->UnicastOnly ) {
			if (iface->AdvSendAdvert) {
				/* send a final advertisement with zero Router Lifetime */
				iface->cease_adv = 1;
				send_ra_forall(iface, NULL);
			}
		}
	}
}