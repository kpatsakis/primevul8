_gdm_display_set_is_local (GdmDisplay     *self,
                           gboolean        is_local)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        g_debug ("GdmDisplay: local: %s", is_local? "yes" : "no");
        priv->is_local = is_local;
}