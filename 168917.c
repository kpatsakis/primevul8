handle_client_state_waiting_for_agree_unix_fd(DBusAuth         *auth,
                                              DBusAuthCommand   command,
                                              const DBusString *args)
{
  switch (command)
    {
    case DBUS_AUTH_COMMAND_AGREE_UNIX_FD:
      _dbus_assert(auth->unix_fd_possible);
      auth->unix_fd_negotiated = TRUE;
      _dbus_verbose("Successfully negotiated UNIX FD passing\n");
      return send_begin (auth);

    case DBUS_AUTH_COMMAND_ERROR:
      _dbus_assert(auth->unix_fd_possible);
      auth->unix_fd_negotiated = FALSE;
      _dbus_verbose("Failed to negotiate UNIX FD passing\n");
      return send_begin (auth);

    case DBUS_AUTH_COMMAND_OK:
    case DBUS_AUTH_COMMAND_DATA:
    case DBUS_AUTH_COMMAND_REJECTED:
    case DBUS_AUTH_COMMAND_AUTH:
    case DBUS_AUTH_COMMAND_CANCEL:
    case DBUS_AUTH_COMMAND_BEGIN:
    case DBUS_AUTH_COMMAND_UNKNOWN:
    case DBUS_AUTH_COMMAND_NEGOTIATE_UNIX_FD:
    default:
      return send_error (auth, "Unknown command");
    }
}