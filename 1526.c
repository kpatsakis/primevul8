int callback_glewlwyd_manage_plugin_module (const struct _u_request * request, struct _u_response * response, void * plugin_data) {
  struct config_elements * config = (struct config_elements *)plugin_data;
  json_t * j_search_module, * j_result, * j_result2;
  
  j_search_module = get_plugin_module(config, u_map_get(request->map_url, "name"));
  if (check_result_value(j_search_module, G_OK)) {
    if (0 == o_strcmp("enable", u_map_get(request->map_url, "action"))) {
      j_result = manage_plugin_module(config, u_map_get(request->map_url, "name"), GLEWLWYD_MODULE_ACTION_START);
      if (check_result_value(j_result, G_ERROR_PARAM)) {
        if (json_object_get(j_result, "error") != NULL) {
          ulfius_set_json_body_response(response, 400, json_object_get(j_result, "error"));
        } else {
          response->status = 400;
        }
      } else if (!check_result_value(j_result, G_OK)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_manage_plugin_module - Error manage_plugin_module enable");
        response->status = 500;
      }
      json_decref(j_result);
    } else if (0 == o_strcmp("disable", u_map_get(request->map_url, "action"))) {
      j_result = manage_plugin_module(config, u_map_get(request->map_url, "name"), GLEWLWYD_MODULE_ACTION_STOP);
      if (check_result_value(j_result, G_ERROR_PARAM)) {
        if (json_object_get(j_result, "error") != NULL) {
          ulfius_set_json_body_response(response, 400, json_object_get(j_result, "error"));
        } else {
          response->status = 400;
        }
      } else if (!check_result_value(j_result, G_OK)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_manage_plugin_module - Error manage_plugin_module disable");
        response->status = 500;
      }
      json_decref(j_result);
    } else if (0 == o_strcmp("reset", u_map_get(request->map_url, "action"))) {
      j_result = manage_plugin_module(config, u_map_get(request->map_url, "name"), GLEWLWYD_MODULE_ACTION_STOP);
      if (check_result_value(j_result, G_ERROR_PARAM)) {
        if (json_object_get(j_result, "error") != NULL) {
          ulfius_set_json_body_response(response, 400, json_object_get(j_result, "error"));
        } else {
          response->status = 400;
        }
      } else if (!check_result_value(j_result, G_OK)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_manage_plugin_module - Error manage_plugin_module reset (1)");
        response->status = 500;
      } else {
        j_result2 = manage_plugin_module(config, u_map_get(request->map_url, "name"), GLEWLWYD_MODULE_ACTION_START);
        if (check_result_value(j_result2, G_ERROR_PARAM)) {
          if (json_object_get(j_result2, "error") != NULL) {
            ulfius_set_json_body_response(response, 400, json_object_get(j_result2, "error"));
          } else {
            response->status = 400;
          }
        } else if (!check_result_value(j_result2, G_OK)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_manage_plugin_module - Error manage_plugin_module reset (1)");
          response->status = 500;
        }
        json_decref(j_result2);
      }
      json_decref(j_result);
    } else {
      response->status = 400;
    }
  } else if (check_result_value(j_search_module, G_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_manage_plugin_module - Error get_plugin_module");
    response->status = 500;
  }
  json_decref(j_search_module);
  return U_CALLBACK_CONTINUE;
}