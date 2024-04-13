dirvote_publish_consensus(void)
{
  int i;

  /* Now remember all the other consensuses as if we were a directory cache. */
  for (i = 0; i < N_CONSENSUS_FLAVORS; ++i) {
    pending_consensus_t *pending = &pending_consensuses[i];
    const char *name;
    name = networkstatus_get_flavor_name(i);
    tor_assert(name);
    if (!pending->consensus ||
      networkstatus_check_consensus_signature(pending->consensus, 1)<0) {
      log_warn(LD_DIR, "Not enough info to publish pending %s consensus",name);
      continue;
    }

    if (networkstatus_set_current_consensus(pending->body, name, 0, NULL))
      log_warn(LD_DIR, "Error publishing %s consensus", name);
    else
      log_notice(LD_DIR, "Published %s consensus", name);
  }

  return 0;
}