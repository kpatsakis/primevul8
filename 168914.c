send_ok (DBusAuth *auth)
{
  int orig_len;

  orig_len = _dbus_string_get_length (&auth->outgoing);
  
  if (_dbus_string_append (&auth->outgoing, "OK ") &&
      _dbus_string_copy (& DBUS_AUTH_SERVER (auth)->guid,
                         0,
                         &auth->outgoing,
                         _dbus_string_get_length (&auth->outgoing)) &&
      _dbus_string_append (&auth->outgoing, "\r\n"))
    {
      goto_state (auth, &server_state_waiting_for_begin);
      return TRUE;
    }
  else
    {
      _dbus_string_set_length (&auth->outgoing, orig_len);
      return FALSE;
    }
}