static void red_sasl_handle_auth_steplen(void *opaque)
{
    RedSASLAuth *auth = (RedSASLAuth*) opaque;

    auth->len = GUINT32_FROM_LE(auth->len);
    uint32_t len = auth->len;
    spice_debug("Got steplen %d", len);
    if (len > SASL_DATA_MAX_LEN) {
        spice_warning("Too much SASL data %d", len);
        return red_sasl_async_result((RedSASLAuth*) opaque, auth->mechname ? RED_SASL_ERROR_INVALID_DATA : RED_SASL_ERROR_GENERIC);
    }

    auth->data = (char*) g_realloc(auth->data, len);
    red_stream_async_read(auth->stream, (uint8_t *)auth->data, len,
                          red_sasl_handle_auth_step, auth);
}