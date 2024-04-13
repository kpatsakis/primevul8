send_begin (DBusAuth         *auth)
{

  if (!_dbus_string_append (&auth->outgoing,
                            "BEGIN\r\n"))
    return FALSE;

  goto_state (auth, &common_state_authenticated);
  return TRUE;
}