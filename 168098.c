vote_routerstatus_find_microdesc_hash(char *digest256_out,
                                      const vote_routerstatus_t *vrs,
                                      int method,
                                      digest_algorithm_t alg)
{
  /* XXXX only returns the sha256 method. */
  const vote_microdesc_hash_t *h;
  char mstr[64];
  size_t mlen;
  char dstr[64];

  tor_snprintf(mstr, sizeof(mstr), "%d", method);
  mlen = strlen(mstr);
  tor_snprintf(dstr, sizeof(dstr), " %s=",
               crypto_digest_algorithm_get_name(alg));

  for (h = vrs->microdesc; h; h = h->next) {
    const char *cp = h->microdesc_hash_line;
    size_t num_len;
    /* cp looks like \d+(,\d+)* (digesttype=val )+ .  Let's hunt for mstr in
     * the first part. */
    while (1) {
      num_len = strspn(cp, "1234567890");
      if (num_len == mlen && fast_memeq(mstr, cp, mlen)) {
        /* This is the line. */
        char buf[BASE64_DIGEST256_LEN+1];
        /* XXXX ignores extraneous stuff if the digest is too long.  This
         * seems harmless enough, right? */
        cp = strstr(cp, dstr);
        if (!cp)
          return -1;
        cp += strlen(dstr);
        strlcpy(buf, cp, sizeof(buf));
        return digest256_from_base64(digest256_out, buf);
      }
      if (num_len == 0 || cp[num_len] != ',')
        break;
      cp += num_len + 1;
    }
  }
  return -1;
}