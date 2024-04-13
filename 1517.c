int callback_glewlwyd_delete_user (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_search_user;
  
  j_search_user = get_user(config, u_map_get(request->map_url, "username"), u_map_get(request->map_url, "source"));
  if (check_result_value(j_search_user, G_OK)) {
    if (delete_user(config, u_map_get(request->map_url, "username"), json_string_value(json_object_get(json_object_get(j_search_user, "user"), "source"))) != G_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_delete_user - Error delete_user");
      response->status = 500;
    } else {
      y_log_message(Y_LOG_LEVEL_INFO, "Event - User '%s' removed", u_map_get(request->map_url, "username"));
    }
  } else if (check_result_value(j_search_user, G_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_delete_user - Error get_user");
    response->status = 500;
  }
  json_decref(j_search_user);
  return U_CALLBACK_CONTINUE;
}