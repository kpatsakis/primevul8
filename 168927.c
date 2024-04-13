_dbus_auth_needs_encoding (DBusAuth *auth)
{
  if (auth->state != &common_state_authenticated)
    return FALSE;
  
  if (auth->mech != NULL)
    {
      if (DBUS_AUTH_IS_CLIENT (auth))
        return auth->mech->client_encode_func != NULL;
      else
        return auth->mech->server_encode_func != NULL;
    }
  else
    return FALSE;
}