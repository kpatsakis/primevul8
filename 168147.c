dirvote_perform_vote(void)
{
  crypto_pk_t *key = get_my_v3_authority_signing_key();
  authority_cert_t *cert = get_my_v3_authority_cert();
  networkstatus_t *ns;
  char *contents;
  pending_vote_t *pending_vote;
  time_t now = time(NULL);

  int status;
  const char *msg = "";

  if (!cert || !key) {
    log_warn(LD_NET, "Didn't find key/certificate to generate v3 vote");
    return -1;
  } else if (cert->expires < now) {
    log_warn(LD_NET, "Can't generate v3 vote with expired certificate");
    return -1;
  }
  if (!(ns = dirserv_generate_networkstatus_vote_obj(key, cert)))
    return -1;

  contents = format_networkstatus_vote(key, ns);
  networkstatus_vote_free(ns);
  if (!contents)
    return -1;

  pending_vote = dirvote_add_vote(contents, &msg, &status);
  tor_free(contents);
  if (!pending_vote) {
    log_warn(LD_DIR, "Couldn't store my own vote! (I told myself, '%s'.)",
             msg);
    return -1;
  }

  directory_post_to_dirservers(DIR_PURPOSE_UPLOAD_VOTE,
                               ROUTER_PURPOSE_GENERAL,
                               V3_DIRINFO,
                               pending_vote->vote_body->dir,
                               pending_vote->vote_body->dir_len, 0);
  log_notice(LD_DIR, "Vote posted.");
  return 0;
}