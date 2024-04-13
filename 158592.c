exim_gssapi_error_defer(uschar *store_reset_point,
    OM_uint32 major, OM_uint32 minor,
    const char *format, ...)
{
  va_list ap;
  OM_uint32 maj_stat, min_stat;
  OM_uint32 msgcontext = 0;
  gss_buffer_desc status_string;
  gstring * g;

  HDEBUG(D_auth)
    {
    va_start(ap, format);
    g = string_vformat(NULL, TRUE, format, ap);
    va_end(ap);
    }

  auth_defer_msg = NULL;

  do {
    maj_stat = gss_display_status(&min_stat,
        major, GSS_C_GSS_CODE, GSS_C_NO_OID, &msgcontext, &status_string);

    if (!auth_defer_msg)
      auth_defer_msg = string_copy(US status_string.value);

    HDEBUG(D_auth) debug_printf("heimdal %s: %.*s\n",
        string_from_gstring(g), (int)status_string.length,
	CS status_string.value);
    gss_release_buffer(&min_stat, &status_string);

  } while (msgcontext != 0);

  if (store_reset_point)
    store_reset(store_reset_point);
  return DEFER;
}