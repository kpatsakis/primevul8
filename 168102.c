voting_schedule_free(voting_schedule_t *voting_schedule_to_free)
{
  if (!voting_schedule_to_free)
    return;
  tor_free(voting_schedule_to_free);
}