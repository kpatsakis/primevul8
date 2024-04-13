_gnutls_compression_is_ok (gnutls_compression_method_t algorithm)
{
  ssize_t ret = -1;
  GNUTLS_COMPRESSION_ALG_LOOP (ret = p->id);
  if (ret >= 0)
    ret = 0;
  else
    ret = 1;
  return ret;
}