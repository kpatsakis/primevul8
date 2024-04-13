_gdm_display_set_session_id (GdmDisplay     *self,
                             const char     *session_id)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        g_debug ("GdmDisplay: session id: %s", session_id);
        g_free (priv->session_id);
        priv->session_id = g_strdup (session_id);
}