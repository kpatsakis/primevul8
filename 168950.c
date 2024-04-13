_dbus_auth_bytes_sent (DBusAuth *auth,
                       int       bytes_sent)
{
  _dbus_verbose ("%s: Sent %d bytes of: %s\n",
                 DBUS_AUTH_NAME (auth),
                 bytes_sent,
                 _dbus_string_get_const_data (&auth->outgoing));
  
  _dbus_string_delete (&auth->outgoing,
                       0, bytes_sent);
}