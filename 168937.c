handle_client_state_waiting_for_data (DBusAuth         *auth,
                                      DBusAuthCommand   command,
                                      const DBusString *args)
{
  _dbus_assert (auth->mech != NULL);
 
  switch (command)
    {
    case DBUS_AUTH_COMMAND_DATA:
      return process_data (auth, args, auth->mech->client_data_func);

    case DBUS_AUTH_COMMAND_REJECTED:
      return process_rejected (auth, args);

    case DBUS_AUTH_COMMAND_OK:
      return process_ok(auth, args);

    case DBUS_AUTH_COMMAND_ERROR:
      return send_cancel (auth);

    case DBUS_AUTH_COMMAND_AUTH:
    case DBUS_AUTH_COMMAND_CANCEL:
    case DBUS_AUTH_COMMAND_BEGIN:
    case DBUS_AUTH_COMMAND_UNKNOWN:
    case DBUS_AUTH_COMMAND_NEGOTIATE_UNIX_FD:
    case DBUS_AUTH_COMMAND_AGREE_UNIX_FD:
    default:
      return send_error (auth, "Unknown command");
    }
}