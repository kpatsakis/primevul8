exim_heimdal_error_debug(const char *label,
    krb5_context context, krb5_error_code err)
{
  const char *kerrsc;
  kerrsc = krb5_get_error_message(context, err);
  debug_printf("heimdal %s: %s\n", label, kerrsc ? kerrsc : "unknown error");
  krb5_free_error_message(context, kerrsc);
}