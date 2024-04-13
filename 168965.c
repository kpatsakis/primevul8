send_error (DBusAuth *auth, const char *message)
{
  return _dbus_string_append_printf (&auth->outgoing,
                                     "ERROR \"%s\"\r\n", message);
}