_gdm_display_set_session_registered (GdmDisplay     *self,
                                     gboolean        registered)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        g_debug ("GdmDisplay: session registered: %s", registered? "yes" : "no");
        priv->session_registered = registered;
}