auth_heimdal_gssapi_init(auth_instance *ablock)
{
  krb5_context context;
  krb5_keytab keytab;
  krb5_kt_cursor cursor;
  krb5_keytab_entry entry;
  krb5_error_code krc;
  char *principal, *enctype_s;
  const char *k_keytab_typed_name = NULL;
  auth_heimdal_gssapi_options_block *ob =
    (auth_heimdal_gssapi_options_block *)(ablock->options_block);

  ablock->server = FALSE;
  ablock->client = FALSE;

  if (!ob->server_service || !*ob->server_service) {
    HDEBUG(D_auth) debug_printf("heimdal: missing server_service\n");
    return;
  }

  krc = krb5_init_context(&context);
  if (krc != 0) {
    int kerr = errno;
    HDEBUG(D_auth) debug_printf("heimdal: failed to initialise krb5 context: %s\n",
        strerror(kerr));
    return;
  }

  if (ob->server_keytab) {
    k_keytab_typed_name = CCS string_sprintf("file:%s", expand_string(ob->server_keytab));
    HDEBUG(D_auth) debug_printf("heimdal: using keytab %s\n", k_keytab_typed_name);
    krc = krb5_kt_resolve(context, k_keytab_typed_name, &keytab);
    if (krc) {
      HDEBUG(D_auth) exim_heimdal_error_debug("krb5_kt_resolve", context, krc);
      return;
    }
  } else {
    HDEBUG(D_auth) debug_printf("heimdal: using system default keytab\n");
    krc = krb5_kt_default(context, &keytab);
    if (krc) {
      HDEBUG(D_auth) exim_heimdal_error_debug("krb5_kt_default", context, krc);
      return;
    }
  }

  HDEBUG(D_auth) {
    /* http://www.h5l.org/manual/HEAD/krb5/krb5_keytab_intro.html */
    krc = krb5_kt_start_seq_get(context, keytab, &cursor);
    if (krc)
      exim_heimdal_error_debug("krb5_kt_start_seq_get", context, krc);
    else {
      while ((krc = krb5_kt_next_entry(context, keytab, &entry, &cursor)) == 0) {
        principal = enctype_s = NULL;
        krb5_unparse_name(context, entry.principal, &principal);
        krb5_enctype_to_string(context, entry.keyblock.keytype, &enctype_s);
        debug_printf("heimdal: keytab principal: %s  vno=%d  type=%s\n",
            principal ? principal : "??",
            entry.vno,
            enctype_s ? enctype_s : "??");
        free(principal);
        free(enctype_s);
        krb5_kt_free_entry(context, &entry);
      }
      krc = krb5_kt_end_seq_get(context, keytab, &cursor);
      if (krc)
        exim_heimdal_error_debug("krb5_kt_end_seq_get", context, krc);
    }
  }

  krc = krb5_kt_close(context, keytab);
  if (krc)
    HDEBUG(D_auth) exim_heimdal_error_debug("krb5_kt_close", context, krc);

  krb5_free_context(context);

  /* RFC 4121 section 5.2, SHOULD support 64K input buffers */
  if (big_buffer_size < (64 * 1024)) {
    uschar *newbuf;
    big_buffer_size = 64 * 1024;
    newbuf = store_malloc(big_buffer_size);
    store_free(big_buffer);
    big_buffer = newbuf;
  }

  ablock->server = TRUE;
}