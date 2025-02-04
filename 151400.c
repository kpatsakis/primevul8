clock_update(
	struct peer *peer	/* peer structure pointer */
	)
{
	double	dtemp;
	l_fp	now;
#ifdef HAVE_LIBSCF_H
	char	*fmri;
#endif /* HAVE_LIBSCF_H */

	/*
	 * Update the system state variables. We do this very carefully,
	 * as the poll interval might need to be clamped differently.
	 */
	sys_peer = peer;
	sys_epoch = peer->epoch;
	if (sys_poll < peer->minpoll)
		sys_poll = peer->minpoll;
	if (sys_poll > peer->maxpoll)
		sys_poll = peer->maxpoll;
	poll_update(peer, sys_poll);
	sys_stratum = min(peer->stratum + 1, STRATUM_UNSPEC);
	if (peer->stratum == STRATUM_REFCLOCK ||
	    peer->stratum == STRATUM_UNSPEC)
		sys_refid = peer->refid;
	else
		sys_refid = addr2refid(&peer->srcadr);
	/*
	 * Root Dispersion (E) is defined (in RFC 5905) as:
	 *
	 * E = p.epsilon_r + p.epsilon + p.psi + PHI*(s.t - p.t) + |THETA|
	 *
	 * where:
	 *  p.epsilon_r is the PollProc's root dispersion
	 *  p.epsilon   is the PollProc's dispersion
	 *  p.psi       is the PollProc's jitter
	 *  THETA       is the combined offset
	 *
	 * NB: Think Hard about where these numbers come from and
	 * what they mean.  When did peer->update happen?  Has anything
	 * interesting happened since then?  What values are the most
	 * defensible?  Why?
	 *
	 * DLM thinks this equation is probably the best of all worse choices.
	 */
	dtemp	= peer->rootdisp
		+ peer->disp
		+ sys_jitter
		+ clock_phi * (current_time - peer->update)
		+ fabs(sys_offset);

	if (dtemp > sys_mindisp)
		sys_rootdisp = dtemp;
	else
		sys_rootdisp = sys_mindisp;
	sys_rootdelay = peer->delay + peer->rootdelay;
	sys_reftime = peer->dst;

#ifdef DEBUG
	if (debug)
		printf(
		    "clock_update: at %lu sample %lu associd %d\n",
		    current_time, peer->epoch, peer->associd);
#endif

	/*
	 * Comes now the moment of truth. Crank the clock discipline and
	 * see what comes out.
	 */
	switch (local_clock(peer, sys_offset)) {

	/*
	 * Clock exceeds panic threshold. Life as we know it ends.
	 */
	case -1:
#ifdef HAVE_LIBSCF_H
		/*
		 * For Solaris enter the maintenance mode.
		 */
		if ((fmri = getenv("SMF_FMRI")) != NULL) {
			if (smf_maintain_instance(fmri, 0) < 0) {
				printf("smf_maintain_instance: %s\n",
				    scf_strerror(scf_error()));
				exit(1);
			}
			/*
			 * Sleep until SMF kills us.
			 */
			for (;;)
				pause();
		}
#endif /* HAVE_LIBSCF_H */
		exit (-1);
		/* not reached */

	/*
	 * Clock was stepped. Flush all time values of all peers.
	 */
	case 2:
		clear_all();
		set_sys_leap(LEAP_NOTINSYNC);
		sys_stratum = STRATUM_UNSPEC;
		memcpy(&sys_refid, "STEP", 4);
		sys_rootdelay = 0;
		sys_rootdisp = 0;
		L_CLR(&sys_reftime);
		sys_jitter = LOGTOD(sys_precision);
		leapsec_reset_frame();
		break;

	/*
	 * Clock was slewed. Handle the leapsecond stuff.
	 */
	case 1:

		/*
		 * If this is the first time the clock is set, reset the
		 * leap bits. If crypto, the timer will goose the setup
		 * process.
		 */
		if (sys_leap == LEAP_NOTINSYNC) {
			set_sys_leap(LEAP_NOWARNING);
#ifdef AUTOKEY
			if (crypto_flags)
				crypto_update();
#endif	/* AUTOKEY */
			/*
			 * If our parent process is waiting for the
			 * first clock sync, send them home satisfied.
			 */
#ifdef HAVE_WORKING_FORK
			if (waitsync_fd_to_close != -1) {
				close(waitsync_fd_to_close);
				waitsync_fd_to_close = -1;
				DPRINTF(1, ("notified parent --wait-sync is done\n"));
			}
#endif /* HAVE_WORKING_FORK */

		}

		/*
		 * If there is no leap second pending and the number of
		 * survivor leap bits is greater than half the number of
		 * survivors, try to schedule a leap for the end of the
		 * current month. (This only works if no leap second for
		 * that range is in the table, so doing this more than
		 * once is mostly harmless.)
		 */
		if (leapsec == LSPROX_NOWARN) {
			if (leap_vote_ins > leap_vote_del
			    && leap_vote_ins > sys_survivors / 2) {
				get_systime(&now);
				leapsec_add_dyn(TRUE, now.l_ui, NULL);
			}
			if (leap_vote_del > leap_vote_ins
			    && leap_vote_del > sys_survivors / 2) {
				get_systime(&now);
				leapsec_add_dyn(FALSE, now.l_ui, NULL);
			}
		}
		break;

	/*
	 * Popcorn spike or step threshold exceeded. Pretend it never
	 * happened.
	 */
	default:
		break;
	}
}