_gnutls_compression_get_comp_level (gnutls_compression_method_t algorithm)
{
  int ret = -1;
  /* avoid prefix */
  GNUTLS_COMPRESSION_ALG_LOOP (ret = p->comp_level);
  return ret;
}