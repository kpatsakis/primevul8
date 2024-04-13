static void red_sasl_handle_auth_mechlen(void *opaque)
{
    RedSASLAuth *auth = (RedSASLAuth*) opaque;

    auth->len = GUINT32_FROM_LE(auth->len);
    uint32_t len = auth->len;
    if (len < 1 || len > SASL_MAX_MECHNAME_LEN) {
        spice_warning("Got bad client mechname len %d", len);
        return red_sasl_async_result(auth, RED_SASL_ERROR_GENERIC);
    }

    auth->mechname = (char*) g_malloc(len + 1);

    spice_debug("Wait for client mechname");
    red_stream_async_read(auth->stream, (uint8_t *)auth->mechname, len,
                          red_sasl_handle_auth_mechname, auth);
}