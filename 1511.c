int callback_glewlwyd_delete_api_key (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  
  if (disable_api_key(config, u_map_get(request->map_url, "key_hash")) != G_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_delete_api_key - Error disable_api_key");
    response->status = 500;
  } else {
    y_log_message(Y_LOG_LEVEL_INFO, "Event - API key disabled by user '%s'", json_string_value(json_object_get((json_t *)response->shared_data, "username")));
  }
  return U_CALLBACK_CONTINUE;
}