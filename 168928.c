_dbus_auth_needs_decoding (DBusAuth *auth)
{
  if (auth->state != &common_state_authenticated)
    return FALSE;
    
  if (auth->mech != NULL)
    {
      if (DBUS_AUTH_IS_CLIENT (auth))
        return auth->mech->client_decode_func != NULL;
      else
        return auth->mech->server_decode_func != NULL;
    }
  else
    return FALSE;
}