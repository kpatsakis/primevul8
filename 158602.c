auth_heimdal_gssapi_client(
  auth_instance *ablock,                 /* authenticator block */
  void * sx,				 /* connection */
  int timeout,                           /* command timeout */
  uschar *buffer,                        /* buffer for reading response */
  int buffsize)                          /* size of buffer */
{
  HDEBUG(D_auth)
    debug_printf("Client side NOT IMPLEMENTED: you should not see this!\n");
  /* NOT IMPLEMENTED */
  return FAIL;
}