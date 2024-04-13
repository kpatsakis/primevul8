handle_server_data_cookie_sha1_mech (DBusAuth         *auth,
                                     const DBusString *data)
{
  if (auth->cookie_id < 0)
    return sha1_handle_first_client_response (auth, data);
  else
    return sha1_handle_second_client_response (auth, data);
}