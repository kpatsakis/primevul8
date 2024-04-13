_gnutls_mac_is_ok (gnutls_mac_algorithm_t algorithm)
{
  ssize_t ret = -1;
  GNUTLS_HASH_ALG_LOOP (ret = p->id);
  if (ret >= 0)
    ret = 0;
  else
    ret = 1;
  return ret;
}