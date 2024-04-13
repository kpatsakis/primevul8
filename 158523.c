auth_heimdal_gssapi_server(auth_instance *ablock, uschar *initial_data)
{
  gss_name_t gclient = GSS_C_NO_NAME;
  gss_name_t gserver = GSS_C_NO_NAME;
  gss_cred_id_t gcred = GSS_C_NO_CREDENTIAL;
  gss_ctx_id_t gcontext = GSS_C_NO_CONTEXT;
  uschar *ex_server_str;
  gss_buffer_desc gbufdesc = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc gbufdesc_in = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc gbufdesc_out = GSS_C_EMPTY_BUFFER;
  gss_OID mech_type;
  OM_uint32 maj_stat, min_stat;
  int step, error_out, i;
  uschar *tmp1, *tmp2, *from_client;
  auth_heimdal_gssapi_options_block *ob =
    (auth_heimdal_gssapi_options_block *)(ablock->options_block);
  BOOL handled_empty_ir;
  uschar *store_reset_point;
  uschar *keytab;
  uschar sasl_config[4];
  uschar requested_qop;

  store_reset_point = store_get(0);

  HDEBUG(D_auth)
    debug_printf("heimdal: initialising auth context for %s\n", ablock->name);

  /* Construct our gss_name_t gserver describing ourselves */
  tmp1 = expand_string(ob->server_service);
  tmp2 = expand_string(ob->server_hostname);
  ex_server_str = string_sprintf("%s@%s", tmp1, tmp2);
  gbufdesc.value = (void *) ex_server_str;
  gbufdesc.length = Ustrlen(ex_server_str);
  maj_stat = gss_import_name(&min_stat,
      &gbufdesc, GSS_C_NT_HOSTBASED_SERVICE, &gserver);
  if (GSS_ERROR(maj_stat))
    return exim_gssapi_error_defer(store_reset_point, maj_stat, min_stat,
        "gss_import_name(%s)", CS gbufdesc.value);

  /* Use a specific keytab, if specified */
  if (ob->server_keytab) {
    keytab = expand_string(ob->server_keytab);
    maj_stat = gsskrb5_register_acceptor_identity(CCS keytab);
    if (GSS_ERROR(maj_stat))
      return exim_gssapi_error_defer(store_reset_point, maj_stat, min_stat,
          "registering keytab \"%s\"", keytab);
    HDEBUG(D_auth)
      debug_printf("heimdal: using keytab \"%s\"\n", keytab);
  }

  /* Acquire our credentials */
  maj_stat = gss_acquire_cred(&min_stat,
      gserver,             /* desired name */
      0,                   /* time */
      GSS_C_NULL_OID_SET,  /* desired mechs */
      GSS_C_ACCEPT,        /* cred usage */
      &gcred,              /* handle */
      NULL                 /* actual mechs */,
      NULL                 /* time rec */);
  if (GSS_ERROR(maj_stat))
    return exim_gssapi_error_defer(store_reset_point, maj_stat, min_stat,
        "gss_acquire_cred(%s)", ex_server_str);

  maj_stat = gss_release_name(&min_stat, &gserver);

  HDEBUG(D_auth) debug_printf("heimdal: have server credentials.\n");

  /* Loop talking to client */
  step = 0;
  from_client = initial_data;
  handled_empty_ir = FALSE;
  error_out = OK;

  /* buffer sizes: auth_get_data() uses big_buffer, which we grow per
  GSSAPI RFC in _init, if needed, to meet the SHOULD size of 64KB.
  (big_buffer starts life at the MUST size of 16KB). */

  /* step values
  0: getting initial data from client to feed into GSSAPI
  1: iterating for as long as GSS_S_CONTINUE_NEEDED
  2: GSS_S_COMPLETE, SASL wrapping for authz and qop to send to client
  3: unpick final auth message from client
  4: break/finish (non-step)
  */
  while (step < 4) {
    switch (step) {
      case 0:
        if (!from_client || *from_client == '\0') {
          if (handled_empty_ir) {
            HDEBUG(D_auth) debug_printf("gssapi: repeated empty input, grr.\n");
            error_out = BAD64;
            goto ERROR_OUT;
          } else {
            HDEBUG(D_auth) debug_printf("gssapi: missing initial response, nudging.\n");
            error_out = auth_get_data(&from_client, US"", 0);
            if (error_out != OK)
              goto ERROR_OUT;
            handled_empty_ir = TRUE;
            continue;
          }
        }
        /* We should now have the opening data from the client, base64-encoded. */
        step += 1;
        HDEBUG(D_auth) debug_printf("heimdal: have initial client data\n");
        break;

      case 1:
        gbufdesc_in.length = b64decode(from_client, USS &gbufdesc_in.value);
        if (gclient) {
          maj_stat = gss_release_name(&min_stat, &gclient);
          gclient = GSS_C_NO_NAME;
        }
        maj_stat = gss_accept_sec_context(&min_stat,
            &gcontext,          /* context handle */
            gcred,              /* acceptor cred handle */
            &gbufdesc_in,       /* input from client */
            GSS_C_NO_CHANNEL_BINDINGS,  /* XXX fixme: use the channel bindings from GnuTLS */
            &gclient,           /* client identifier */
            &mech_type,         /* mechanism in use */
            &gbufdesc_out,      /* output to send to client */
            NULL,               /* return flags */
            NULL,               /* time rec */
            NULL                /* delegated cred_handle */
            );
        if (GSS_ERROR(maj_stat)) {
          exim_gssapi_error_defer(NULL, maj_stat, min_stat,
              "gss_accept_sec_context()");
          error_out = FAIL;
          goto ERROR_OUT;
        }
        if (gbufdesc_out.length != 0) {
          error_out = auth_get_data(&from_client,
              gbufdesc_out.value, gbufdesc_out.length);
          if (error_out != OK)
            goto ERROR_OUT;

          gss_release_buffer(&min_stat, &gbufdesc_out);
          EmptyBuf(gbufdesc_out);
        }
        if (maj_stat == GSS_S_COMPLETE) {
          step += 1;
          HDEBUG(D_auth) debug_printf("heimdal: GSS complete\n");
        } else {
          HDEBUG(D_auth) debug_printf("heimdal: need more data\n");
        }
        break;

      case 2:
        memset(sasl_config, 0xFF, 4);
        /* draft-ietf-sasl-gssapi-06.txt defines bitmasks for first octet
        0x01 No security layer
        0x02 Integrity protection
        0x04 Confidentiality protection

        The remaining three octets are the maximum buffer size for wrapped
        content. */
        sasl_config[0] = 0x01;  /* Exim does not wrap/unwrap SASL layers after auth */
        gbufdesc.value = (void *) sasl_config;
        gbufdesc.length = 4;
        maj_stat = gss_wrap(&min_stat,
            gcontext,
            0,                    /* conf_req_flag: integrity only */
            GSS_C_QOP_DEFAULT,    /* qop requested */
            &gbufdesc,            /* message to protect */
            NULL,                 /* conf_state: no confidentiality applied */
            &gbufdesc_out         /* output buffer */
            );
        if (GSS_ERROR(maj_stat)) {
          exim_gssapi_error_defer(NULL, maj_stat, min_stat,
              "gss_wrap(SASL state after auth)");
          error_out = FAIL;
          goto ERROR_OUT;
        }

        HDEBUG(D_auth) debug_printf("heimdal SASL: requesting QOP with no security layers\n");

        error_out = auth_get_data(&from_client,
            gbufdesc_out.value, gbufdesc_out.length);
        if (error_out != OK)
          goto ERROR_OUT;

        gss_release_buffer(&min_stat, &gbufdesc_out);
        EmptyBuf(gbufdesc_out);
        step += 1;
        break;

      case 3:
        gbufdesc_in.length = b64decode(from_client, USS &gbufdesc_in.value);
        maj_stat = gss_unwrap(&min_stat,
            gcontext,
            &gbufdesc_in,       /* data from client */
            &gbufdesc_out,      /* results */
            NULL,               /* conf state */
            NULL                /* qop state */
            );
        if (GSS_ERROR(maj_stat)) {
          exim_gssapi_error_defer(NULL, maj_stat, min_stat,
              "gss_unwrap(final SASL message from client)");
          error_out = FAIL;
          goto ERROR_OUT;
        }
        if (gbufdesc_out.length < 4) {
          HDEBUG(D_auth)
            debug_printf("gssapi: final message too short; "
                "need flags, buf sizes and optional authzid\n");
          error_out = FAIL;
          goto ERROR_OUT;
        }

        requested_qop = (CS gbufdesc_out.value)[0];
        if ((requested_qop & 0x01) == 0) {
          HDEBUG(D_auth)
            debug_printf("gssapi: client requested security layers (%x)\n",
                (unsigned int) requested_qop);
          error_out = FAIL;
          goto ERROR_OUT;
        }

        for (i = 0; i < AUTH_VARS; i++) auth_vars[i] = NULL;
        expand_nmax = 0;

        /* Identifiers:
        The SASL provided identifier is an unverified authzid.
        GSSAPI provides us with a verified identifier, but it might be empty
        for some clients.
        */

        /* $auth2 is authzid requested at SASL layer */
        if (gbufdesc_out.length > 4) {
          expand_nlength[2] = gbufdesc_out.length - 4;
          auth_vars[1] = expand_nstring[2] =
            string_copyn((US gbufdesc_out.value) + 4, expand_nlength[2]);
          expand_nmax = 2;
        }

        gss_release_buffer(&min_stat, &gbufdesc_out);
        EmptyBuf(gbufdesc_out);

        /* $auth1 is GSSAPI display name */
        maj_stat = gss_display_name(&min_stat,
            gclient,
            &gbufdesc_out,
            &mech_type);
        if (GSS_ERROR(maj_stat)) {
          auth_vars[1] = expand_nstring[2] = NULL;
          expand_nmax = 0;
          exim_gssapi_error_defer(NULL, maj_stat, min_stat,
              "gss_display_name(client identifier)");
          error_out = FAIL;
          goto ERROR_OUT;
        }

        expand_nlength[1] = gbufdesc_out.length;
        auth_vars[0] = expand_nstring[1] =
          string_copyn(gbufdesc_out.value, gbufdesc_out.length);

        if (expand_nmax == 0) { /* should be: authzid was empty */
          expand_nmax = 2;
          expand_nlength[2] = expand_nlength[1];
          auth_vars[1] = expand_nstring[2] = string_copyn(expand_nstring[1], expand_nlength[1]);
          HDEBUG(D_auth)
            debug_printf("heimdal SASL: empty authzid, set to dup of GSSAPI display name\n");
        }

        HDEBUG(D_auth)
          debug_printf("heimdal SASL: happy with client request\n"
             "  auth1 (verified GSSAPI display-name): \"%s\"\n"
             "  auth2 (unverified SASL requested authzid): \"%s\"\n",
             auth_vars[0], auth_vars[1]);

        step += 1;
        break;

    } /* switch */
  } /* while step */


ERROR_OUT:
  maj_stat = gss_release_cred(&min_stat, &gcred);
  if (gclient) {
    gss_release_name(&min_stat, &gclient);
    gclient = GSS_C_NO_NAME;
  }
  if (gbufdesc_out.length) {
    gss_release_buffer(&min_stat, &gbufdesc_out);
    EmptyBuf(gbufdesc_out);
  }
  if (gcontext != GSS_C_NO_CONTEXT) {
    gss_delete_sec_context(&min_stat, &gcontext, GSS_C_NO_BUFFER);
  }

  store_reset(store_reset_point);

  if (error_out != OK)
    return error_out;

  /* Auth succeeded, check server_condition */
  return auth_check_serv_cond(ablock);
}