_gnutls_map_kx_get_kx (gnutls_credentials_type_t type, int server)
{
  gnutls_kx_algorithm_t ret = -1;

  if (server)
    {
      GNUTLS_KX_MAP_ALG_LOOP_SERVER (ret = p->algorithm);
    }
  else
    {
      GNUTLS_KX_MAP_ALG_LOOP_SERVER (ret = p->algorithm);
    }
  return ret;
}