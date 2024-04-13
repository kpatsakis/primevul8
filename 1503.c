int callback_glewlwyd_add_client (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_client, * j_client_valid, * j_search_client, * j_body;
  
  j_client = ulfius_get_json_body_request(request, NULL);
  if (j_client != NULL) {
    j_client_valid = is_client_valid(config, NULL, j_client, 1, u_map_get(request->map_url, "source"));
    if (check_result_value(j_client_valid, G_OK)) {
      j_search_client = get_client(config, json_string_value(json_object_get(j_client, "client_id")), u_map_get(request->map_url, "source"));
      if (check_result_value(j_search_client, G_ERROR_NOT_FOUND)) {
        if (add_client(config, j_client, u_map_get(request->map_url, "source")) != G_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_add_client - Error add_client");
          response->status = 500;
        } else {
          y_log_message(Y_LOG_LEVEL_INFO, "Event - Client '%s' added", json_string_value(json_object_get(j_client, "client_id")));
        }
      } else if (check_result_value(j_search_client, G_OK)) {
        j_body = json_pack("{s[s]}", "error", "client_id already exists");
        ulfius_set_json_body_response(response, 400, j_body);
        json_decref(j_body);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_add_client - Error get_client");
        response->status = 500;
      }
      json_decref(j_search_client);
    } else if (check_result_value(j_client_valid, G_ERROR_PARAM)) {
      if (json_object_get(j_client_valid, "error") != NULL) {
        ulfius_set_json_body_response(response, 400, json_object_get(j_client_valid, "error"));
      } else {
        response->status = 400;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_add_client - Error is_client_valid");
      response->status = 500;
    }
    json_decref(j_client_valid);
  } else {
    response->status = 400;
  }
  json_decref(j_client);
  return U_CALLBACK_CONTINUE;
}