gnutls_compression_get_name (gnutls_compression_method_t algorithm)
{
  const char *ret = NULL;

  /* avoid prefix */
  GNUTLS_COMPRESSION_ALG_LOOP (ret = p->name + sizeof ("GNUTLS_COMP_") - 1);

  return ret;
}