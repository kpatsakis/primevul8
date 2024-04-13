dirvote_format_microdesc_vote_line(char *out_buf, size_t out_buf_len,
                                   const microdesc_t *md,
                                   int consensus_method_low,
                                   int consensus_method_high)
{
  ssize_t ret = -1;
  char d64[BASE64_DIGEST256_LEN+1];
  char *microdesc_consensus_methods =
    make_consensus_method_list(consensus_method_low,
                               consensus_method_high,
                               ",");
  tor_assert(microdesc_consensus_methods);

  if (digest256_to_base64(d64, md->digest)<0)
    goto out;

  if (tor_snprintf(out_buf, out_buf_len, "m %s sha256=%s\n",
                   microdesc_consensus_methods, d64)<0)
    goto out;

  ret = strlen(out_buf);

 out:
  tor_free(microdesc_consensus_methods);
  return ret;
}