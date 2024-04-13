_gdm_display_set_session_type (GdmDisplay *self,
                               const char *session_type)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        g_debug ("GdmDisplay: session type: %s", session_type);
        g_free (priv->session_type);
        priv->session_type = g_strdup (session_type);
}