int callback_glewlwyd_scheme_check_forbid_profile (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_param = ulfius_get_json_body_request(request, NULL), * j_scheme = get_user_auth_scheme_module(config, json_string_value(json_object_get(j_param, "scheme_name")));
  int ret = U_CALLBACK_CONTINUE;
  
  if (check_result_value(j_scheme, G_OK)) {
    if (json_object_get(json_object_get(j_scheme, "module"), "forbid_user_profile") == json_true()) {
      response->status = 403;
      ret = U_CALLBACK_COMPLETE;
    }
  } else if (check_result_value(j_scheme, G_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_scheme_check_forbid_profile - Error auth_register_get_user_scheme");
    response->status = 500;
  }
  json_decref(j_param);
  json_decref(j_scheme);
  return ret;
}