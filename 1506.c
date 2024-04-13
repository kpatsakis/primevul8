int callback_glewlwyd_get_user (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_user;
  
  j_user = get_user(config, u_map_get(request->map_url, "username"), u_map_get(request->map_url, "source"));
  if (check_result_value(j_user, G_OK)) {
    ulfius_set_json_body_response(response, 200, json_object_get(j_user, "user"));
  } else if (check_result_value(j_user, G_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_get_user - Error j_user");
    response->status = 500;
  }
  json_decref(j_user);
  return U_CALLBACK_CONTINUE;
}