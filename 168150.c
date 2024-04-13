dirvote_fetch_missing_votes(void)
{
  smartlist_t *missing_fps = smartlist_new();
  char *resource;

  SMARTLIST_FOREACH_BEGIN(router_get_trusted_dir_servers(),
                          dir_server_t *, ds) {
      if (!(ds->type & V3_DIRINFO))
        continue;
      if (!dirvote_get_vote(ds->v3_identity_digest,
                            DGV_BY_ID|DGV_INCLUDE_PENDING)) {
        char *cp = tor_malloc(HEX_DIGEST_LEN+1);
        base16_encode(cp, HEX_DIGEST_LEN+1, ds->v3_identity_digest,
                      DIGEST_LEN);
        smartlist_add(missing_fps, cp);
      }
  } SMARTLIST_FOREACH_END(ds);

  if (!smartlist_len(missing_fps)) {
    smartlist_free(missing_fps);
    return;
  }
  {
    char *tmp = smartlist_join_strings(missing_fps, " ", 0, NULL);
    log_notice(LOG_NOTICE, "We're missing votes from %d authorities (%s). "
               "Asking every other authority for a copy.",
               smartlist_len(missing_fps), tmp);
    tor_free(tmp);
  }
  resource = smartlist_join_strings(missing_fps, "+", 0, NULL);
  directory_get_from_all_authorities(DIR_PURPOSE_FETCH_STATUS_VOTE,
                                     0, resource);
  tor_free(resource);
  SMARTLIST_FOREACH(missing_fps, char *, cp, tor_free(cp));
  smartlist_free(missing_fps);
}