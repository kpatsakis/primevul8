int callback_glewlwyd_user_get_profile (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_session;
  char * session_uid, expires[129];
  time_t now;
  struct tm ts;
  
  time(&now);
  now += GLEWLWYD_DEFAULT_SESSION_EXPIRATION_COOKIE;
  gmtime_r(&now, &ts);
  strftime(expires, 128, "%a, %d %b %Y %T %Z", &ts);
  if (!o_strlen(u_map_get(request->map_url, "username"))) {
    session_uid = get_session_id(config, request);
    if (session_uid != NULL && o_strlen(session_uid)) {
      j_session = get_users_for_session(config, session_uid);
      if (check_result_value(j_session, G_OK)) {
        ulfius_set_json_body_response(response, 200, json_object_get(j_session, "session"));
        ulfius_add_cookie_to_response(response, config->session_key, session_uid, expires, 0, config->cookie_domain, "/", config->cookie_secure, 0);
      } else if (check_result_value(j_session, G_ERROR_NOT_FOUND)) {
        response->status = 401;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_get_session - Error get_current_user_for_session");
        response->status = 500;
      }
      json_decref(j_session);
    } else {
      response->status = 401;
    }
    o_free(session_uid);
  } else {
    // Can't impersonate this endpoint
    response->status = 400;
  }
  
  return U_CALLBACK_CONTINUE;
}