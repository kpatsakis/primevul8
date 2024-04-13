handle_server_shutdown_cookie_sha1_mech (DBusAuth *auth)
{
  auth->cookie_id = -1;  
  _dbus_string_set_length (&auth->challenge, 0);
}