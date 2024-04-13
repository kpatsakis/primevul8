int callback_glewlwyd_delete_client (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_search_client;
  
  j_search_client = get_client(config, u_map_get(request->map_url, "client_id"), u_map_get(request->map_url, "source"));
  if (check_result_value(j_search_client, G_OK)) {
    if (delete_client(config, u_map_get(request->map_url, "client_id"), json_string_value(json_object_get(json_object_get(j_search_client, "client"), "source"))) != G_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_delete_client - Error delete_client");
      response->status = 500;
    } else {
      y_log_message(Y_LOG_LEVEL_INFO, "Event - Client '%s' removed", u_map_get(request->map_url, "client_id"));
    }
  } else if (check_result_value(j_search_client, G_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_delete_client - Error get_client");
    response->status = 500;
  }
  json_decref(j_search_client);
  return U_CALLBACK_CONTINUE;
}