_gnutls_cipher_get_export_flag (gnutls_cipher_algorithm_t algorithm)
{				/* In bytes */
  size_t ret = 0;
  GNUTLS_ALG_LOOP (ret = p->export_flag);
  return ret;

}