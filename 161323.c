_gdm_display_set_allow_timed_login (GdmDisplay     *self,
                                    gboolean        allow_timed_login)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        g_debug ("GdmDisplay: allow timed login: %s", allow_timed_login? "yes" : "no");
        priv->allow_timed_login = allow_timed_login;
}