send_data (DBusAuth *auth, DBusString *data)
{
  int old_len;

  if (data == NULL || _dbus_string_get_length (data) == 0)
    return _dbus_string_append (&auth->outgoing, "DATA\r\n");
  else
    {
      old_len = _dbus_string_get_length (&auth->outgoing);
      if (!_dbus_string_append (&auth->outgoing, "DATA "))
        goto out;

      if (!_dbus_string_hex_encode (data, 0, &auth->outgoing,
                                    _dbus_string_get_length (&auth->outgoing)))
        goto out;

      if (!_dbus_string_append (&auth->outgoing, "\r\n"))
        goto out;

      return TRUE;

    out:
      _dbus_string_set_length (&auth->outgoing, old_len);

      return FALSE;
    }
}