MOCK_IMPL(const char *,
dirvote_get_pending_detached_signatures, (void))
{
  return pending_consensus_signatures;
}