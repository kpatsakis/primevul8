_gnutls_compression_get_mem_level (gnutls_compression_method_t algorithm)
{
  int ret = -1;
  /* avoid prefix */
  GNUTLS_COMPRESSION_ALG_LOOP (ret = p->mem_level);
  return ret;
}