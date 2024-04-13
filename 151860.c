bool red_sasl_start_auth(RedStream *stream, RedSaslResult result_cb, void *result_opaque)
{
    const char *mechlist = NULL;
    sasl_security_properties_t secprops;
    int err;
    char *localAddr, *remoteAddr;
    int mechlistlen;
    RedSASL *sasl = &stream->priv->sasl;
    RedSASLAuth *auth;

    if (!(localAddr = red_stream_get_local_address(stream))) {
        goto error;
    }

    if (!(remoteAddr = red_stream_get_remote_address(stream))) {
        g_free(localAddr);
        goto error;
    }

    err = sasl_server_new("spice",
                          NULL, /* FQDN - just delegates to gethostname */
                          NULL, /* User realm */
                          localAddr,
                          remoteAddr,
                          NULL, /* Callbacks, not needed */
                          SASL_SUCCESS_DATA,
                          &sasl->conn);
    g_free(localAddr);
    g_free(remoteAddr);
    localAddr = remoteAddr = NULL;

    if (err != SASL_OK) {
        spice_warning("sasl context setup failed %d (%s)",
                    err, sasl_errstring(err, NULL, NULL));
        sasl->conn = NULL;
        goto error;
    }

    /* Inform SASL that we've got an external SSF layer from TLS */
    if (stream->priv->ssl) {
        sasl_ssf_t ssf;

        ssf = SSL_get_cipher_bits(stream->priv->ssl, NULL);
        err = sasl_setprop(sasl->conn, SASL_SSF_EXTERNAL, &ssf);
        if (err != SASL_OK) {
            spice_warning("cannot set SASL external SSF %d (%s)",
                        err, sasl_errstring(err, NULL, NULL));
            goto error_dispose;
        }
    } else {
        sasl->wantSSF = 1;
    }

    memset(&secprops, 0, sizeof secprops);
    /* Inform SASL that we've got an external SSF layer from TLS */
    if (stream->priv->ssl) {
        /* If we've got TLS (or UNIX domain sock), we don't care about SSF */
        secprops.min_ssf = 0;
        secprops.max_ssf = 0;
        secprops.maxbufsize = 8192;
        secprops.security_flags = 0;
    } else {
        /* Plain TCP, better get an SSF layer */
        secprops.min_ssf = 56; /* Good enough to require kerberos */
        secprops.max_ssf = 100000; /* Arbitrary big number */
        secprops.maxbufsize = 8192;
        /* Forbid any anonymous or trivially crackable auth */
        secprops.security_flags =
            SASL_SEC_NOANONYMOUS | SASL_SEC_NOPLAINTEXT;
    }

    err = sasl_setprop(sasl->conn, SASL_SEC_PROPS, &secprops);
    if (err != SASL_OK) {
        spice_warning("cannot set SASL security props %d (%s)",
                      err, sasl_errstring(err, NULL, NULL));
        goto error_dispose;
    }

    err = sasl_listmech(sasl->conn,
                        NULL, /* Don't need to set user */
                        ",", /* Prefix */
                        ",", /* Separator */
                        ",", /* Suffix */
                        &mechlist,
                        NULL,
                        NULL);
    if (err != SASL_OK || mechlist == NULL) {
        spice_warning("cannot list SASL mechanisms %d (%s)",
                      err, sasl_errdetail(sasl->conn));
        goto error_dispose;
    }

    spice_debug("Available mechanisms for client: '%s'", mechlist);

    mechlistlen = strlen(mechlist);
    if (!red_stream_write_u32_le(stream, mechlistlen)
        || !red_stream_write_all(stream, mechlist, mechlistlen)) {
        spice_warning("SASL mechanisms write error");
        goto error;
    }

    auth = g_new0(RedSASLAuth, 1);
    auth->stream = stream;
    auth->result_cb = result_cb;
    auth->result_opaque = result_opaque;
    auth->saved_error_cb = stream->priv->async_read.error;
    auth->mechlist = g_strdup(mechlist);

    spice_debug("Wait for client mechname length");
    red_stream_set_async_error_handler(stream, red_sasl_error);
    red_stream_async_read(stream, (uint8_t *)&auth->len, sizeof(uint32_t),
                          red_sasl_handle_auth_mechlen, auth);

    return true;

error_dispose:
    sasl_dispose(&sasl->conn);
    sasl->conn = NULL;
error:
    return false;
}