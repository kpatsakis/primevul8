int callback_glewlwyd_user_auth_trigger (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_param = ulfius_get_json_body_request(request, NULL), * j_result = NULL;

  if (j_param != NULL) {
    if (json_string_length(json_object_get(j_param, "scheme_type")) && json_string_length(json_object_get(j_param, "scheme_name"))) {
      if (json_string_length(json_object_get(j_param, "username"))) {
        j_result = auth_trigger_user_scheme(config, json_string_value(json_object_get(j_param, "scheme_type")), json_string_value(json_object_get(j_param, "scheme_name")), json_string_value(json_object_get(j_param, "username")), json_object_get(j_param, "value"), request);
        if (check_result_value(j_result, G_ERROR_PARAM)) {
          ulfius_set_string_body_response(response, 400, "bad scheme response");
        } else if (check_result_value(j_result, G_ERROR_NOT_FOUND)) {
          response->status = 404;
        } else if (check_result_value(j_result, G_ERROR_UNAUTHORIZED)) {
          response->status = 401;
        } else if (check_result_value(j_result, G_OK)) {
          if (json_object_get(j_result, "trigger") != NULL) {
            ulfius_set_json_body_response(response, 200, json_object_get(j_result, "trigger"));
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_auth_trigger - Error auth_trigger_user_scheme");
          response->status = 500;
        }
        json_decref(j_result);
      } else {
        j_result = auth_trigger_identify_scheme(config, json_string_value(json_object_get(j_param, "scheme_type")), json_string_value(json_object_get(j_param, "scheme_name")), json_object_get(j_param, "value"), request);
        if (check_result_value(j_result, G_ERROR_PARAM)) {
          ulfius_set_string_body_response(response, 400, "bad scheme response");
        } else if (check_result_value(j_result, G_ERROR_NOT_FOUND)) {
          response->status = 404;
        } else if (check_result_value(j_result, G_ERROR_UNAUTHORIZED)) {
          response->status = 401;
        } else if (check_result_value(j_result, G_OK)) {
          if (json_object_get(j_result, "trigger") != NULL) {
            ulfius_set_json_body_response(response, 200, json_object_get(j_result, "trigger"));
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_auth_trigger - Error auth_trigger_identify_scheme");
          response->status = 500;
        }
        json_decref(j_result);
      }
    } else {
      ulfius_set_string_body_response(response, 400, "scheme is mandatory");
    }
  } else {
    ulfius_set_string_body_response(response, 400, "Input parameters must be in JSON format");
  }
  json_decref(j_param);
  return U_CALLBACK_CONTINUE;
}