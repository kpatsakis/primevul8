handle_server_state_waiting_for_begin (DBusAuth         *auth,
                                       DBusAuthCommand   command,
                                       const DBusString *args)
{
  switch (command)
    {
    case DBUS_AUTH_COMMAND_AUTH:
      return send_error (auth, "Sent AUTH while expecting BEGIN");

    case DBUS_AUTH_COMMAND_DATA:
      return send_error (auth, "Sent DATA while expecting BEGIN");

    case DBUS_AUTH_COMMAND_BEGIN:
      goto_state (auth, &common_state_authenticated);
      return TRUE;

    case DBUS_AUTH_COMMAND_NEGOTIATE_UNIX_FD:
      if (auth->unix_fd_possible)
        return send_agree_unix_fd(auth);
      else
        return send_error(auth, "Unix FD passing not supported, not authenticated or otherwise not possible");

    case DBUS_AUTH_COMMAND_REJECTED:
    case DBUS_AUTH_COMMAND_OK:
    case DBUS_AUTH_COMMAND_UNKNOWN:
    case DBUS_AUTH_COMMAND_AGREE_UNIX_FD:
    default:
      return send_error (auth, "Unknown command");

    case DBUS_AUTH_COMMAND_CANCEL:
    case DBUS_AUTH_COMMAND_ERROR:
      return send_rejected (auth);
    }
}