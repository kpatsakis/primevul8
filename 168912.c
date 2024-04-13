handle_server_state_waiting_for_data  (DBusAuth         *auth,
                                       DBusAuthCommand   command,
                                       const DBusString *args)
{
  switch (command)
    {
    case DBUS_AUTH_COMMAND_AUTH:
      return send_error (auth, "Sent AUTH while another AUTH in progress");

    case DBUS_AUTH_COMMAND_CANCEL:
    case DBUS_AUTH_COMMAND_ERROR:
      return send_rejected (auth);

    case DBUS_AUTH_COMMAND_DATA:
      return process_data (auth, args, auth->mech->server_data_func);

    case DBUS_AUTH_COMMAND_BEGIN:
      goto_state (auth, &common_state_need_disconnect);
      return TRUE;

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