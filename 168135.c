authority_cert_dup(authority_cert_t *cert)
{
  authority_cert_t *out = tor_malloc(sizeof(authority_cert_t));
  tor_assert(cert);

  memcpy(out, cert, sizeof(authority_cert_t));
  /* Now copy pointed-to things. */
  out->cache_info.signed_descriptor_body =
    tor_strndup(cert->cache_info.signed_descriptor_body,
                cert->cache_info.signed_descriptor_len);
  out->cache_info.saved_location = SAVED_NOWHERE;
  out->identity_key = crypto_pk_dup_key(cert->identity_key);
  out->signing_key = crypto_pk_dup_key(cert->signing_key);

  return out;
}