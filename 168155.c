get_voting_schedule(const or_options_t *options, time_t now, int severity)
{
  int interval, vote_delay, dist_delay;
  time_t start;
  time_t end;
  networkstatus_t *consensus;
  voting_schedule_t *new_voting_schedule;

  new_voting_schedule = tor_malloc_zero(sizeof(voting_schedule_t));

  consensus = networkstatus_get_live_consensus(now);

  if (consensus) {
    interval = (int)( consensus->fresh_until - consensus->valid_after );
    vote_delay = consensus->vote_seconds;
    dist_delay = consensus->dist_seconds;
  } else {
    interval = options->TestingV3AuthInitialVotingInterval;
    vote_delay = options->TestingV3AuthInitialVoteDelay;
    dist_delay = options->TestingV3AuthInitialDistDelay;
  }

  tor_assert(interval > 0);

  if (vote_delay + dist_delay > interval/2)
    vote_delay = dist_delay = interval / 4;

  start = new_voting_schedule->interval_starts =
    dirvote_get_start_of_next_interval(now,interval,
                                      options->TestingV3AuthVotingStartOffset);
  end = dirvote_get_start_of_next_interval(start+1, interval,
                                      options->TestingV3AuthVotingStartOffset);

  tor_assert(end > start);

  new_voting_schedule->fetch_missing_signatures = start - (dist_delay/2);
  new_voting_schedule->voting_ends = start - dist_delay;
  new_voting_schedule->fetch_missing_votes =
    start - dist_delay - (vote_delay/2);
  new_voting_schedule->voting_starts = start - dist_delay - vote_delay;

  {
    char tbuf[ISO_TIME_LEN+1];
    format_iso_time(tbuf, new_voting_schedule->interval_starts);
    tor_log(severity, LD_DIR,"Choosing expected valid-after time as %s: "
            "consensus_set=%d, interval=%d",
            tbuf, consensus?1:0, interval);
  }

  return new_voting_schedule;
}