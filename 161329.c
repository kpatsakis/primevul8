_gdm_display_set_session_class (GdmDisplay *self,
                                const char *session_class)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        g_debug ("GdmDisplay: session class: %s", session_class);
        g_free (priv->session_class);
        priv->session_class = g_strdup (session_class);
}