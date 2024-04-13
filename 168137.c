dirvote_create_microdescriptor(const routerinfo_t *ri, int consensus_method)
{
  microdesc_t *result = NULL;
  char *key = NULL, *summary = NULL, *family = NULL;
  size_t keylen;
  smartlist_t *chunks = smartlist_new();
  char *output = NULL;

  if (crypto_pk_write_public_key_to_string(ri->onion_pkey, &key, &keylen)<0)
    goto done;
  summary = policy_summarize(ri->exit_policy, AF_INET);
  if (ri->declared_family)
    family = smartlist_join_strings(ri->declared_family, " ", 0, NULL);

  smartlist_add_asprintf(chunks, "onion-key\n%s", key);

  if (consensus_method >= MIN_METHOD_FOR_NTOR_KEY &&
      ri->onion_curve25519_pkey) {
    char kbuf[128];
    base64_encode(kbuf, sizeof(kbuf),
                  (const char*)ri->onion_curve25519_pkey->public_key,
                  CURVE25519_PUBKEY_LEN, BASE64_ENCODE_MULTILINE);
    smartlist_add_asprintf(chunks, "ntor-onion-key %s", kbuf);
  }

  if (consensus_method >= MIN_METHOD_FOR_A_LINES &&
      !tor_addr_is_null(&ri->ipv6_addr) && ri->ipv6_orport)
    smartlist_add_asprintf(chunks, "a %s\n",
                           fmt_addrport(&ri->ipv6_addr, ri->ipv6_orport));

  if (family)
    smartlist_add_asprintf(chunks, "family %s\n", family);

  if (summary && strcmp(summary, "reject 1-65535"))
    smartlist_add_asprintf(chunks, "p %s\n", summary);

  if (consensus_method >= MIN_METHOD_FOR_P6_LINES &&
      ri->ipv6_exit_policy) {
    /* XXXX+++ This doesn't match proposal 208, which says these should
     * be taken unchanged from the routerinfo.  That's bogosity, IMO:
     * the proposal should have said to do this instead.*/
    char *p6 = write_short_policy(ri->ipv6_exit_policy);
    if (p6 && strcmp(p6, "reject 1-65535"))
      smartlist_add_asprintf(chunks, "p6 %s\n", p6);
    tor_free(p6);
  }

  if (consensus_method >= MIN_METHOD_FOR_ID_HASH_IN_MD) {
    char idbuf[ED25519_BASE64_LEN+1];
    const char *keytype;
    if (consensus_method >= MIN_METHOD_FOR_ED25519_ID_IN_MD &&
        ri->cache_info.signing_key_cert &&
        ri->cache_info.signing_key_cert->signing_key_included) {
      keytype = "ed25519";
      ed25519_public_to_base64(idbuf,
                               &ri->cache_info.signing_key_cert->signing_key);
    } else {
      keytype = "rsa1024";
      digest_to_base64(idbuf, ri->cache_info.identity_digest);
    }
    smartlist_add_asprintf(chunks, "id %s %s\n", keytype, idbuf);
  }

  output = smartlist_join_strings(chunks, "", 0, NULL);

  {
    smartlist_t *lst = microdescs_parse_from_string(output,
                                                    output+strlen(output), 0,
                                                    SAVED_NOWHERE, NULL);
    if (smartlist_len(lst) != 1) {
      log_warn(LD_DIR, "We generated a microdescriptor we couldn't parse.");
      SMARTLIST_FOREACH(lst, microdesc_t *, md, microdesc_free(md));
      smartlist_free(lst);
      goto done;
    }
    result = smartlist_get(lst, 0);
    smartlist_free(lst);
  }

 done:
  tor_free(output);
  tor_free(key);
  tor_free(summary);
  tor_free(family);
  if (chunks) {
    SMARTLIST_FOREACH(chunks, char *, cp, tor_free(cp));
    smartlist_free(chunks);
  }
  return result;
}