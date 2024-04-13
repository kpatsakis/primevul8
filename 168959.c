send_agree_unix_fd (DBusAuth *auth)
{
  _dbus_assert(auth->unix_fd_possible);

  auth->unix_fd_negotiated = TRUE;
  _dbus_verbose("Agreed to UNIX FD passing\n");

  if (!_dbus_string_append (&auth->outgoing,
                            "AGREE_UNIX_FD\r\n"))
    return FALSE;

  goto_state (auth, &server_state_waiting_for_begin);
  return TRUE;
}