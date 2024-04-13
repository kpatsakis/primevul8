int callback_glewlwyd_user_delete_session (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_session, * j_cur_session;
  char * session_uid = get_session_id(config, request), expires[129];
  size_t index;
  time_t now;
  struct tm ts;
  
  time(&now);
  now += GLEWLWYD_DEFAULT_SESSION_EXPIRATION_COOKIE;
  gmtime_r(&now, &ts);
  strftime(expires, 128, "%a, %d %b %Y %T %Z", &ts);
  if (session_uid != NULL && o_strlen(session_uid)) {
    j_session = get_users_for_session(config, session_uid);
    if (check_result_value(j_session, G_ERROR_NOT_FOUND)) {
      response->status = 404;
    } else if (!check_result_value(j_session, G_OK)) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_delete_session - Error get_current_user_for_session");
      response->status = 500;
    } else {
      if (u_map_get(request->map_url, "username") != NULL) {
        json_array_foreach(json_object_get(j_session, "session"), index, j_cur_session) {
          if (0 == o_strcasecmp(u_map_get(request->map_url, "username"), json_string_value(json_object_get(j_cur_session, "username")))) {
            if (user_session_delete(config, session_uid, u_map_get(request->map_url, "username")) != G_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_delete_session - Error user_session_delete");
              response->status = 500;
            }
          }
        }
        if (json_array_size(json_object_get(j_session, "session")) == 1) {
          // Delete session cookie on the client browser
          ulfius_add_cookie_to_response(response, config->session_key, "", expires, 0, config->cookie_domain, "/", config->cookie_secure, 0);
        } else {
          ulfius_add_cookie_to_response(response, config->session_key, session_uid, expires, 0, config->cookie_domain, "/", config->cookie_secure, 0);
        }
      } else {
        if (user_session_delete(config, session_uid, NULL) != G_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_delete_session - Error user_session_delete");
          response->status = 500;
        }
        // Delete session cookie on the client browser
        ulfius_add_cookie_to_response(response, config->session_key, "", expires, 0, config->cookie_domain, "/", config->cookie_secure, 0);
      }
    }
    json_decref(j_session);
  } else {
    response->status = 401;
  }
  o_free(session_uid);
  
  return U_CALLBACK_CONTINUE;
}