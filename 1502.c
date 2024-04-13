int callback_glewlwyd_delete_scope (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_search_scope;
  
  j_search_scope = get_scope(config, u_map_get(request->map_url, "scope"));
  if (check_result_value(j_search_scope, G_OK)) {
    if (delete_scope(config, u_map_get(request->map_url, "scope")) != G_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_delete_scope - Error delete_scope");
      response->status = 500;
    } else {
      y_log_message(Y_LOG_LEVEL_INFO, "Event - Scope '%s' removed", u_map_get(request->map_url, "scope"));
    }
  } else if (check_result_value(j_search_scope, G_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_delete_scope - Error get_scope");
    response->status = 500;
  }
  json_decref(j_search_scope);
  return U_CALLBACK_CONTINUE;
}