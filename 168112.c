networkstatus_format_signatures(networkstatus_t *consensus,
                                int for_detached_signatures)
{
  smartlist_t *elements;
  char buf[4096];
  char *result = NULL;
  int n_sigs = 0;
  const consensus_flavor_t flavor = consensus->flavor;
  const char *flavor_name = networkstatus_get_flavor_name(flavor);
  const char *keyword;

  if (for_detached_signatures && flavor != FLAV_NS)
    keyword = "additional-signature";
  else
    keyword = "directory-signature";

  elements = smartlist_new();

  SMARTLIST_FOREACH_BEGIN(consensus->voters, networkstatus_voter_info_t *, v) {
    SMARTLIST_FOREACH_BEGIN(v->sigs, document_signature_t *, sig) {
      char sk[HEX_DIGEST_LEN+1];
      char id[HEX_DIGEST_LEN+1];
      if (!sig->signature || sig->bad_signature)
        continue;
      ++n_sigs;
      base16_encode(sk, sizeof(sk), sig->signing_key_digest, DIGEST_LEN);
      base16_encode(id, sizeof(id), sig->identity_digest, DIGEST_LEN);
      if (flavor == FLAV_NS) {
        smartlist_add_asprintf(elements,
                     "%s %s %s\n-----BEGIN SIGNATURE-----\n",
                     keyword, id, sk);
      } else {
        const char *digest_name =
          crypto_digest_algorithm_get_name(sig->alg);
        smartlist_add_asprintf(elements,
                     "%s%s%s %s %s %s\n-----BEGIN SIGNATURE-----\n",
                     keyword,
                     for_detached_signatures ? " " : "",
                     for_detached_signatures ? flavor_name : "",
                     digest_name, id, sk);
      }
      base64_encode(buf, sizeof(buf), sig->signature, sig->signature_len,
                    BASE64_ENCODE_MULTILINE);
      strlcat(buf, "-----END SIGNATURE-----\n", sizeof(buf));
      smartlist_add(elements, tor_strdup(buf));
    } SMARTLIST_FOREACH_END(sig);
  } SMARTLIST_FOREACH_END(v);

  result = smartlist_join_strings(elements, "", 0, NULL);
  SMARTLIST_FOREACH(elements, char *, cp, tor_free(cp));
  smartlist_free(elements);
  if (!n_sigs)
    tor_free(result);
  return result;
}