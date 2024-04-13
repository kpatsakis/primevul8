_gnutls_kx_auth_struct (gnutls_kx_algorithm_t algorithm)
{
  mod_auth_st *ret = NULL;
  GNUTLS_KX_ALG_LOOP (ret = p->auth_struct);
  return ret;

}