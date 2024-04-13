int callback_glewlwyd_set_scope (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_scope, * j_scope_valid, * j_search_scope;
  
  j_search_scope = get_scope(config, u_map_get(request->map_url, "scope"));
  if (check_result_value(j_search_scope, G_OK)) {
    j_scope = ulfius_get_json_body_request(request, NULL);
    if (j_scope != NULL) {
      j_scope_valid = is_scope_valid(config, j_scope, 0);
      if (check_result_value(j_scope_valid, G_OK)) {
        if (set_scope(config, u_map_get(request->map_url, "scope"), j_scope) != G_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_scope - Error set_scope");
          response->status = 500;
        } else {
          y_log_message(Y_LOG_LEVEL_INFO, "Event - Scope '%s' updated", u_map_get(request->map_url, "scope"));
        }
      } else if (check_result_value(j_scope_valid, G_ERROR_PARAM)) {
        ulfius_set_json_body_response(response, 400, json_object_get(j_scope_valid, "error"));
      } else if (!check_result_value(j_scope_valid, G_OK)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_scope - Error is_scope_valid");
        response->status = 500;
      }
      json_decref(j_scope_valid);
    } else {
      response->status = 400;
    }
    json_decref(j_scope);
  } else if (check_result_value(j_search_scope, G_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_scope - Error get_scope");
    response->status = 500;
  }
  json_decref(j_search_scope);
  return U_CALLBACK_CONTINUE;
}