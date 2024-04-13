list_v3_auth_ids(void)
{
  smartlist_t *known_v3_keys = smartlist_new();
  char *keys;
  SMARTLIST_FOREACH(router_get_trusted_dir_servers(),
                    dir_server_t *, ds,
    if ((ds->type & V3_DIRINFO) &&
        !tor_digest_is_zero(ds->v3_identity_digest))
      smartlist_add(known_v3_keys,
                    tor_strdup(hex_str(ds->v3_identity_digest, DIGEST_LEN))));
  keys = smartlist_join_strings(known_v3_keys, ", ", 0, NULL);
  SMARTLIST_FOREACH(known_v3_keys, char *, cp, tor_free(cp));
  smartlist_free(known_v3_keys);
  return keys;
}