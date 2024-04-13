dirvote_act(const or_options_t *options, time_t now)
{
  if (!authdir_mode_v3(options))
    return;
  if (!voting_schedule.voting_starts) {
    char *keys = list_v3_auth_ids();
    authority_cert_t *c = get_my_v3_authority_cert();
    log_notice(LD_DIR, "Scheduling voting.  Known authority IDs are %s. "
               "Mine is %s.",
               keys, hex_str(c->cache_info.identity_digest, DIGEST_LEN));
    tor_free(keys);
    dirvote_recalculate_timing(options, now);
  }
  if (voting_schedule.voting_starts < now && !voting_schedule.have_voted) {
    log_notice(LD_DIR, "Time to vote.");
    dirvote_perform_vote();
    voting_schedule.have_voted = 1;
  }
  if (voting_schedule.fetch_missing_votes < now &&
      !voting_schedule.have_fetched_missing_votes) {
    log_notice(LD_DIR, "Time to fetch any votes that we're missing.");
    dirvote_fetch_missing_votes();
    voting_schedule.have_fetched_missing_votes = 1;
  }
  if (voting_schedule.voting_ends < now &&
      !voting_schedule.have_built_consensus) {
    log_notice(LD_DIR, "Time to compute a consensus.");
    dirvote_compute_consensuses();
    /* XXXX We will want to try again later if we haven't got enough
     * votes yet.  Implement this if it turns out to ever happen. */
    voting_schedule.have_built_consensus = 1;
  }
  if (voting_schedule.fetch_missing_signatures < now &&
      !voting_schedule.have_fetched_missing_signatures) {
    log_notice(LD_DIR, "Time to fetch any signatures that we're missing.");
    dirvote_fetch_missing_signatures();
    voting_schedule.have_fetched_missing_signatures = 1;
  }
  if (voting_schedule.interval_starts < now &&
      !voting_schedule.have_published_consensus) {
    log_notice(LD_DIR, "Time to publish the consensus and discard old votes");
    dirvote_publish_consensus();
    dirvote_clear_votes(0);
    voting_schedule.have_published_consensus = 1;
    /* Update our shared random state with the consensus just published. */
    sr_act_post_consensus(
                networkstatus_get_latest_consensus_by_flavor(FLAV_NS));
    /* XXXX We will want to try again later if we haven't got enough
     * signatures yet.  Implement this if it turns out to ever happen. */
    dirvote_recalculate_timing(options, now);
  }
}