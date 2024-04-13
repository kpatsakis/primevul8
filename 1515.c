int callback_glewlwyd_get_module_type_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  UNUSED(request);
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_module_type;
  
  j_module_type = get_module_type_list(config);
  if (check_result_value(j_module_type, G_OK)) {
    ulfius_set_json_body_response(response, 200, json_object_get(j_module_type, "module"));
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_get_module_type_list - Error get_module_type_list");
    response->status = 500;
  }
  json_decref(j_module_type);
  return U_CALLBACK_CONTINUE;
}