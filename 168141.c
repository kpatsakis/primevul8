write_v3_votes_to_disk(const smartlist_t *pending_votes)
{
  smartlist_t *votestrings = smartlist_new();
  char *votefile = NULL;

  SMARTLIST_FOREACH(pending_votes, pending_vote_t *, v,
    {
      sized_chunk_t *c = tor_malloc(sizeof(sized_chunk_t));
      c->bytes = v->vote_body->dir;
      c->len = v->vote_body->dir_len;
      smartlist_add(votestrings, c); /* collect strings to write to disk */
    });

  votefile = get_datadir_fname("v3-status-votes");
  write_chunks_to_file(votefile, votestrings, 0, 0);
  log_debug(LD_DIR, "Wrote votes to disk (%s)!", votefile);

  tor_free(votefile);
  SMARTLIST_FOREACH(votestrings, sized_chunk_t *, c, tor_free(c));
  smartlist_free(votestrings);
}