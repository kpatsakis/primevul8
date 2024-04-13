int callback_glewlwyd_user_auth_register (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_param = ulfius_get_json_body_request(request, NULL), * j_result = NULL;

  if (j_param != NULL) {
    if (json_object_get(j_param, "username") != NULL && json_is_string(json_object_get(j_param, "username")) && json_string_length(json_object_get(j_param, "username"))) {
      if (0 == o_strcasecmp(json_string_value(json_object_get((json_t *)response->shared_data, "username")), json_string_value(json_object_get(j_param, "username")))) {
        if (json_object_get(j_param, "scheme_type") != NULL && json_is_string(json_object_get(j_param, "scheme_type")) && json_string_length(json_object_get(j_param, "scheme_type")) && json_object_get(j_param, "scheme_name") != NULL && json_is_string(json_object_get(j_param, "scheme_name")) && json_string_length(json_object_get(j_param, "scheme_name"))) {
          j_result = auth_register_user_scheme(config, json_string_value(json_object_get(j_param, "scheme_type")), json_string_value(json_object_get(j_param, "scheme_name")), json_string_value(json_object_get(j_param, "username")), 0, json_object_get(j_param, "value"), request);
          if (check_result_value(j_result, G_ERROR_PARAM)) {
            if (json_object_get(j_result, "register") != NULL) {
              ulfius_set_json_body_response(response, 400, json_object_get(j_result, "register"));
            } else {
              ulfius_set_string_body_response(response, 400, "bad scheme response");
            }
          } else if (check_result_value(j_result, G_ERROR_NOT_FOUND)) {
            response->status = 404;
          } else if (check_result_value(j_result, G_ERROR_UNAUTHORIZED)) {
            response->status = 401;
          } else if (check_result_value(j_result, G_OK)) {
            if (json_object_get(j_result, "register") != NULL) {
              ulfius_set_json_body_response(response, 200, json_object_get(j_result, "register"));
            }
            y_log_message(Y_LOG_LEVEL_INFO, "Event - User '%s' registered scheme '%s/%s'", json_string_value(json_object_get(j_param, "username")), json_string_value(json_object_get(j_param, "scheme_type")), json_string_value(json_object_get(j_param, "scheme_name")));
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_auth_register - Error auth_check_user_scheme");
            response->status = 500;
          }
          json_decref(j_result);
        } else {
          ulfius_set_string_body_response(response, 400, "scheme is mandatory");
        }
      } else {
        ulfius_set_string_body_response(response, 400, "username invalid");
      }
    } else {
      ulfius_set_string_body_response(response, 400, "username is mandatory");
    }
  } else {
    ulfius_set_string_body_response(response, 400, "Input parameters must be in JSON format");
  }
  json_decref(j_param);
  return U_CALLBACK_CONTINUE;
}