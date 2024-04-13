handle_server_state_waiting_for_auth  (DBusAuth         *auth,
                                       DBusAuthCommand   command,
                                       const DBusString *args)
{
  switch (command)
    {
    case DBUS_AUTH_COMMAND_AUTH:
      return handle_auth (auth, args);

    case DBUS_AUTH_COMMAND_CANCEL:
    case DBUS_AUTH_COMMAND_DATA:
      return send_error (auth, "Not currently in an auth conversation");

    case DBUS_AUTH_COMMAND_BEGIN:
      goto_state (auth, &common_state_need_disconnect);
      return TRUE;

    case DBUS_AUTH_COMMAND_ERROR:
      return send_rejected (auth);

    case DBUS_AUTH_COMMAND_NEGOTIATE_UNIX_FD:
      return send_error (auth, "Need to authenticate first");

    case DBUS_AUTH_COMMAND_REJECTED:
    case DBUS_AUTH_COMMAND_OK:
    case DBUS_AUTH_COMMAND_UNKNOWN:
    case DBUS_AUTH_COMMAND_AGREE_UNIX_FD:
    default:
      return send_error (auth, "Unknown command");
    }
}