int callback_glewlwyd_options (const struct _u_request * request, struct _u_response * response, void * user_data) {
  UNUSED(request);
  UNUSED(user_data);
  ulfius_add_header_to_response(response, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
  ulfius_add_header_to_response(response, "Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, Bearer, Authorization");
  ulfius_add_header_to_response(response, "Access-Control-Max-Age", "1800");
  return U_CALLBACK_COMPLETE;
}