send_negotiate_unix_fd (DBusAuth *auth)
{
  if (!_dbus_string_append (&auth->outgoing,
                            "NEGOTIATE_UNIX_FD\r\n"))
    return FALSE;

  goto_state (auth, &client_state_waiting_for_agree_unix_fd);
  return TRUE;
}