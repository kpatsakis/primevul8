peer_clear(
	struct peer *peer,		/* peer structure */
	const char *ident		/* tally lights */
	)
{
	u_char	u;

#ifdef AUTOKEY
	/*
	 * If cryptographic credentials have been acquired, toss them to
	 * Valhalla. Note that autokeys are ephemeral, in that they are
	 * tossed immediately upon use. Therefore, the keylist can be
	 * purged anytime without needing to preserve random keys. Note
	 * that, if the peer is purged, the cryptographic variables are
	 * purged, too. This makes it much harder to sneak in some
	 * unauthenticated data in the clock filter.
	 */
	key_expire(peer);
	if (peer->iffval != NULL)
		BN_free(peer->iffval);
	value_free(&peer->cookval);
	value_free(&peer->recval);
	value_free(&peer->encrypt);
	value_free(&peer->sndval);
	if (peer->cmmd != NULL)
		free(peer->cmmd);
	if (peer->subject != NULL)
		free(peer->subject);
	if (peer->issuer != NULL)
		free(peer->issuer);
#endif /* AUTOKEY */

	/*
	 * Clear all values, including the optional crypto values above.
	 */
	memset(CLEAR_TO_ZERO(peer), 0, LEN_CLEAR_TO_ZERO(peer));
	peer->ppoll = peer->maxpoll;
	peer->hpoll = peer->minpoll;
	peer->disp = MAXDISPERSE;
	peer->flash = peer_unfit(peer);
	peer->jitter = LOGTOD(sys_precision);

	/*
	 * If interleave mode, initialize the alternate origin switch.
	 */
	if (peer->flags & FLAG_XLEAVE)
		peer->flip = 1;
	for (u = 0; u < NTP_SHIFT; u++) {
		peer->filter_order[u] = u;
		peer->filter_disp[u] = MAXDISPERSE;
	}
#ifdef REFCLOCK
	if (!(peer->flags & FLAG_REFCLOCK)) {
#endif
		peer->leap = LEAP_NOTINSYNC;
		peer->stratum = STRATUM_UNSPEC;
		memcpy(&peer->refid, ident, 4);
#ifdef REFCLOCK
	}
#endif

	/*
	 * During initialization use the association count to spread out
	 * the polls at one-second intervals. Passive associations'
	 * first poll is delayed by the "discard minimum" to avoid rate
	 * limiting. Other post-startup new or cleared associations
	 * randomize the first poll over the minimum poll interval to
	 * avoid implosion.
	 */
	peer->nextdate = peer->update = peer->outdate = current_time;
	if (initializing) {
		peer->nextdate += peer_associations;
	} else if (MODE_PASSIVE == peer->hmode) {
		peer->nextdate += ntp_minpkt;
	} else {
		peer->nextdate += ntp_random() % peer->minpoll;
	}
#ifdef AUTOKEY
	peer->refresh = current_time + (1 << NTP_REFRESH);
#endif	/* AUTOKEY */
#ifdef DEBUG
	if (debug)
		printf(
		    "peer_clear: at %ld next %ld associd %d refid %s\n",
		    current_time, peer->nextdate, peer->associd,
		    ident);
#endif
}