transmit(
	struct peer *peer	/* peer structure pointer */
	)
{
	u_char	hpoll;

	/*
	 * The polling state machine. There are two kinds of machines,
	 * those that never expect a reply (broadcast and manycast
	 * server modes) and those that do (all other modes). The dance
	 * is intricate...
	 */
	hpoll = peer->hpoll;

	/*
	 * In broadcast mode the poll interval is never changed from
	 * minpoll.
	 */
	if (peer->cast_flags & (MDF_BCAST | MDF_MCAST)) {
		peer->outdate = current_time;
		if (sys_leap != LEAP_NOTINSYNC)
			peer_xmit(peer);
		poll_update(peer, hpoll);
		return;
	}

	/*
	 * In manycast mode we start with unity ttl. The ttl is
	 * increased by one for each poll until either sys_maxclock
	 * servers have been found or the maximum ttl is reached. When
	 * sys_maxclock servers are found we stop polling until one or
	 * more servers have timed out or until less than sys_minclock
	 * associations turn up. In this case additional better servers
	 * are dragged in and preempt the existing ones.  Once every
	 * sys_beacon seconds we are to transmit unconditionally, but
	 * this code is not quite right -- peer->unreach counts polls
	 * and is being compared with sys_beacon, so the beacons happen
	 * every sys_beacon polls.
	 */
	if (peer->cast_flags & MDF_ACAST) {
		peer->outdate = current_time;
		if (peer->unreach > sys_beacon) {
			peer->unreach = 0;
			peer->ttl = 0;
			peer_xmit(peer);
		} else if (sys_survivors < sys_minclock ||
		    peer_associations < sys_maxclock) {
			if (peer->ttl < (u_int32)sys_ttlmax)
				peer->ttl++;
			peer_xmit(peer);
		}
		peer->unreach++;
		poll_update(peer, hpoll);
		return;
	}

	/*
	 * Pool associations transmit unicast solicitations when there
	 * are less than a hard limit of 2 * sys_maxclock associations,
	 * and either less than sys_minclock survivors or less than
	 * sys_maxclock associations.  The hard limit prevents unbounded
	 * growth in associations if the system clock or network quality
	 * result in survivor count dipping below sys_minclock often.
	 * This was observed testing with pool, where sys_maxclock == 12
	 * resulted in 60 associations without the hard limit.  A
	 * similar hard limit on manycastclient ephemeral associations
	 * may be appropriate.
	 */
	if (peer->cast_flags & MDF_POOL) {
		peer->outdate = current_time;
		if ((peer_associations <= 2 * sys_maxclock) &&
		    (peer_associations < sys_maxclock ||
		     sys_survivors < sys_minclock))
			pool_xmit(peer);
		poll_update(peer, hpoll);
		return;
	}

	/*
	 * In unicast modes the dance is much more intricate. It is
	 * designed to back off whenever possible to minimize network
	 * traffic.
	 */
	if (peer->burst == 0) {
		u_char oreach;

		/*
		 * Update the reachability status. If not heard for
		 * three consecutive polls, stuff infinity in the clock
		 * filter.
		 */
		oreach = peer->reach;
		peer->outdate = current_time;
		peer->unreach++;
		peer->reach <<= 1;
		if (!peer->reach) {

			/*
			 * Here the peer is unreachable. If it was
			 * previously reachable raise a trap. Send a
			 * burst if enabled.
			 */
			clock_filter(peer, 0., 0., MAXDISPERSE);
			if (oreach) {
				peer_unfit(peer);
				report_event(PEVNT_UNREACH, peer, NULL);
			}
			if ((peer->flags & FLAG_IBURST) &&
			    peer->retry == 0)
				peer->retry = NTP_RETRY;
		} else {

			/*
			 * Here the peer is reachable. Send a burst if
			 * enabled and the peer is fit.  Reset unreach
			 * for persistent and ephemeral associations.
			 * Unreach is also reset for survivors in
			 * clock_select().
			 */
			hpoll = sys_poll;
			if (!(peer->flags & FLAG_PREEMPT))
				peer->unreach = 0;
			if ((peer->flags & FLAG_BURST) && peer->retry ==
			    0 && !peer_unfit(peer))
				peer->retry = NTP_RETRY;
		}

		/*
		 * Watch for timeout.  If ephemeral, toss the rascal;
		 * otherwise, bump the poll interval. Note the
		 * poll_update() routine will clamp it to maxpoll.
		 * If preemptible and we have more peers than maxclock,
		 * and this peer has the minimum score of preemptibles,
		 * demobilize.
		 */
		if (peer->unreach >= NTP_UNREACH) {
			hpoll++;
			/* ephemeral: no FLAG_CONFIG nor FLAG_PREEMPT */
			if (!(peer->flags & (FLAG_CONFIG | FLAG_PREEMPT))) {
				report_event(PEVNT_RESTART, peer, "timeout");
				peer_clear(peer, "TIME");
				unpeer(peer);
				return;
			}
			if ((peer->flags & FLAG_PREEMPT) &&
			    (peer_associations > sys_maxclock) &&
			    score_all(peer)) {
				report_event(PEVNT_RESTART, peer, "timeout");
				peer_clear(peer, "TIME");
				unpeer(peer);
				return;
			}
		}
	} else {
		peer->burst--;
		if (peer->burst == 0) {

			/*
			 * If ntpdate mode and the clock has not been
			 * set and all peers have completed the burst,
			 * we declare a successful failure.
			 */
			if (mode_ntpdate) {
				peer_ntpdate--;
				if (peer_ntpdate == 0) {
					msyslog(LOG_NOTICE,
					    "ntpd: no servers found");
					if (!msyslog_term)
						printf(
						    "ntpd: no servers found\n");
					exit (0);
				}
			}
		}
	}
	if (peer->retry > 0)
		peer->retry--;

	/*
	 * Do not transmit if in broadcast client mode.
	 */
	if (peer->hmode != MODE_BCLIENT)
		peer_xmit(peer);
	poll_update(peer, hpoll);
}