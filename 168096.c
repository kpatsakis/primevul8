MOCK_IMPL(const char *,
dirvote_get_pending_consensus, (consensus_flavor_t flav))
{
  tor_assert(((int)flav) >= 0 && (int)flav < N_CONSENSUS_FLAVORS);
  return pending_consensuses[flav].body;
}