int callback_glewlwyd_get_api_key_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_api_key_list;
  size_t offset = 0, limit = GLEWLWYD_DEFAULT_LIMIT_SIZE;
  long int l_converted = 0;
  char * endptr = NULL;
  
  if (u_map_get(request->map_url, "offset") != NULL) {
    l_converted = strtol(u_map_get(request->map_url, "offset"), &endptr, 10);
    if (!(*endptr) && l_converted > 0) {
      offset = (size_t)l_converted;
    }
  }
  if (u_map_get(request->map_url, "limit") != NULL) {
    l_converted = strtol(u_map_get(request->map_url, "limit"), &endptr, 10);
    if (!(*endptr) && l_converted >= 0) {
      limit = (size_t)l_converted;
    }
  }
  j_api_key_list = get_api_key_list(config, u_map_get(request->map_url, "pattern"), offset, limit);
  if (check_result_value(j_api_key_list, G_OK)) {
    ulfius_set_json_body_response(response, 200, json_object_get(j_api_key_list, "api_key"));
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_get_api_key_list - Error get_api_key_list");
    response->status = 500;
  }
  json_decref(j_api_key_list);
  return U_CALLBACK_CONTINUE;
}