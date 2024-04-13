get_next_valid_after_time(time_t now)
{
  time_t next_valid_after_time;
  const or_options_t *options = get_options();
  voting_schedule_t *new_voting_schedule =
    get_voting_schedule(options, now, LOG_INFO);
  tor_assert(new_voting_schedule);

  next_valid_after_time = new_voting_schedule->interval_starts;
  voting_schedule_free(new_voting_schedule);

  return next_valid_after_time;
}