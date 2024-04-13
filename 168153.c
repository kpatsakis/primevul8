dirvote_get_preferred_voting_intervals(vote_timing_t *timing_out)
{
  const or_options_t *options = get_options();

  tor_assert(timing_out);

  timing_out->vote_interval = options->V3AuthVotingInterval;
  timing_out->n_intervals_valid = options->V3AuthNIntervalsValid;
  timing_out->vote_delay = options->V3AuthVoteDelay;
  timing_out->dist_delay = options->V3AuthDistDelay;
}