static void red_sasl_error(void *opaque, int err)
{
    RedSASLAuth *auth = (RedSASLAuth*) opaque;
    red_stream_set_async_error_handler(auth->stream, auth->saved_error_cb);
    if (auth->saved_error_cb) {
        auth->saved_error_cb(auth->result_opaque, err);
    }
    red_sasl_auth_free(auth);
}