_dbus_auth_set_mechanisms (DBusAuth    *auth,
                           const char **mechanisms)
{
  char **copy;

  if (mechanisms != NULL)
    {
      copy = _dbus_dup_string_array (mechanisms);
      if (copy == NULL)
        return FALSE;
    }
  else
    copy = NULL;
  
  dbus_free_string_array (auth->allowed_mechs);

  auth->allowed_mechs = copy;

  return TRUE;
}