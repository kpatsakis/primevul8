int callback_glewlwyd_get_plugin_module_list (const struct _u_request * request, struct _u_response * response, void * plugin_data) {
  UNUSED(request);
  struct config_elements * config = (struct config_elements *)plugin_data;
  json_t * j_module;
  
  j_module = get_plugin_module_list(config);
  if (check_result_value(j_module, G_OK)) {
    ulfius_set_json_body_response(response, 200, json_object_get(j_module, "module"));
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_get_plugin_module_list - Error get_plugin_module_list");
    response->status = 500;
  }
  json_decref(j_module);
  return U_CALLBACK_CONTINUE;
}