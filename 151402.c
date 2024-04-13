poll_update(
	struct peer *peer,	/* peer structure pointer */
	u_char	mpoll
	)
{
	u_long	next, utemp;
	u_char	hpoll;

	/*
	 * This routine figures out when the next poll should be sent.
	 * That turns out to be wickedly complicated. One problem is
	 * that sometimes the time for the next poll is in the past when
	 * the poll interval is reduced. We watch out for races here
	 * between the receive process and the poll process.
	 *
	 * Clamp the poll interval between minpoll and maxpoll.
	 */
	hpoll = max(min(peer->maxpoll, mpoll), peer->minpoll);

#ifdef AUTOKEY
	/*
	 * If during the crypto protocol the poll interval has changed,
	 * the lifetimes in the key list are probably bogus. Purge the
	 * the key list and regenerate it later.
	 */
	if ((peer->flags & FLAG_SKEY) && hpoll != peer->hpoll)
		key_expire(peer);
#endif	/* AUTOKEY */
	peer->hpoll = hpoll;

	/*
	 * There are three variables important for poll scheduling, the
	 * current time (current_time), next scheduled time (nextdate)
	 * and the earliest time (utemp). The earliest time is 2 s
	 * seconds, but could be more due to rate management. When
	 * sending in a burst, use the earliest time. When not in a
	 * burst but with a reply pending, send at the earliest time
	 * unless the next scheduled time has not advanced. This can
	 * only happen if multiple replies are pending in the same
	 * response interval. Otherwise, send at the later of the next
	 * scheduled time and the earliest time.
	 *
	 * Now we figure out if there is an override. If a burst is in
	 * progress and we get called from the receive process, just
	 * slink away. If called from the poll process, delay 1 s for a
	 * reference clock, otherwise 2 s.
	 */
	utemp = current_time + max(peer->throttle - (NTP_SHIFT - 1) *
	    (1 << peer->minpoll), ntp_minpkt);
	if (peer->burst > 0) {
		if (peer->nextdate > current_time)
			return;
#ifdef REFCLOCK
		else if (peer->flags & FLAG_REFCLOCK)
			peer->nextdate = current_time + RESP_DELAY;
#endif /* REFCLOCK */
		else
			peer->nextdate = utemp;

#ifdef AUTOKEY
	/*
	 * If a burst is not in progress and a crypto response message
	 * is pending, delay 2 s, but only if this is a new interval.
	 */
	} else if (peer->cmmd != NULL) {
		if (peer->nextdate > current_time) {
			if (peer->nextdate + ntp_minpkt != utemp)
				peer->nextdate = utemp;
		} else {
			peer->nextdate = utemp;
		}
#endif	/* AUTOKEY */

	/*
	 * The ordinary case. If a retry, use minpoll; if unreachable,
	 * use host poll; otherwise, use the minimum of host and peer
	 * polls; In other words, oversampling is okay but
	 * understampling is evil. Use the maximum of this value and the
	 * headway. If the average headway is greater than the headway
	 * threshold, increase the headway by the minimum interval.
	 */
	} else {
		if (peer->retry > 0)
			hpoll = peer->minpoll;
		else if (!(peer->reach))
			hpoll = peer->hpoll;
		else
			hpoll = min(peer->ppoll, peer->hpoll);
#ifdef REFCLOCK
		if (peer->flags & FLAG_REFCLOCK)
			next = 1 << hpoll;
		else
#endif /* REFCLOCK */
			next = ((0x1000UL | (ntp_random() & 0x0ff)) <<
			    hpoll) >> 12;
		next += peer->outdate;
		if (next > utemp)
			peer->nextdate = next;
		else
			peer->nextdate = utemp;
		if (peer->throttle > (1 << peer->minpoll))
			peer->nextdate += ntp_minpkt;
	}
	DPRINTF(2, ("poll_update: at %lu %s poll %d burst %d retry %d head %d early %lu next %lu\n",
		    current_time, ntoa(&peer->srcadr), peer->hpoll,
		    peer->burst, peer->retry, peer->throttle,
		    utemp - current_time, peer->nextdate -
		    current_time));
}