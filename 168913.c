handle_client_state_waiting_for_reject (DBusAuth         *auth,
                                        DBusAuthCommand   command,
                                        const DBusString *args)
{
  switch (command)
    {
    case DBUS_AUTH_COMMAND_REJECTED:
      return process_rejected (auth, args);
      
    case DBUS_AUTH_COMMAND_AUTH:
    case DBUS_AUTH_COMMAND_CANCEL:
    case DBUS_AUTH_COMMAND_DATA:
    case DBUS_AUTH_COMMAND_BEGIN:
    case DBUS_AUTH_COMMAND_OK:
    case DBUS_AUTH_COMMAND_ERROR:
    case DBUS_AUTH_COMMAND_UNKNOWN:
    case DBUS_AUTH_COMMAND_NEGOTIATE_UNIX_FD:
    case DBUS_AUTH_COMMAND_AGREE_UNIX_FD:
    default:
      goto_state (auth, &common_state_need_disconnect);
      return TRUE;
    }
}