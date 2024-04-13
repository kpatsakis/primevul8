int callback_glewlwyd_delete_user_middleware_module (const struct _u_request * request, struct _u_response * response, void * user_middleware_data) {
  struct config_elements * config = (struct config_elements *)user_middleware_data;
  json_t * j_search_module;
  
  j_search_module = get_user_middleware_module(config, u_map_get(request->map_url, "name"));
  if (check_result_value(j_search_module, G_OK)) {
    if (delete_user_middleware_module(config, u_map_get(request->map_url, "name")) != G_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_delete_user_middleware_module - Error delete_user_middleware_module");
      response->status = 500;
    } else {
      y_log_message(Y_LOG_LEVEL_INFO, "Event - User backend module '%s' removed", u_map_get(request->map_url, "name"));
    }
  } else if (check_result_value(j_search_module, G_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_delete_user_middleware_module - Error get_user_middleware_module");
    response->status = 500;
  }
  json_decref(j_search_module);
  return U_CALLBACK_CONTINUE;
}