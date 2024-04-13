_gnutls_compression_get_wbits (gnutls_compression_method_t algorithm)
{
  int ret = -1;
  /* avoid prefix */
  GNUTLS_COMPRESSION_ALG_LOOP (ret = p->window_bits);
  return ret;
}