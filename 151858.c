void red_stream_free(RedStream *s)
{
    if (!s) {
        return;
    }

    red_stream_push_channel_event(s, SPICE_CHANNEL_EVENT_DISCONNECTED);

#if HAVE_SASL
    if (s->priv->sasl.conn) {
        s->priv->sasl.runSSF = s->priv->sasl.wantSSF = 0;
        s->priv->sasl.encodedLength = s->priv->sasl.encodedOffset = 0;
        s->priv->sasl.encoded = NULL;
        sasl_dispose(&s->priv->sasl.conn);
        s->priv->sasl.conn = NULL;
    }
#endif

    if (s->priv->ssl) {
        SSL_free(s->priv->ssl);
    }

    websocket_free(s->priv->ws);

    red_stream_remove_watch(s);
    socket_close(s->socket);

    g_free(s);
}