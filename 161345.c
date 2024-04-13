_gdm_display_set_is_initial (GdmDisplay     *self,
                             gboolean        initial)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        g_debug ("GdmDisplay: initial: %s", initial? "yes" : "no");
        priv->is_initial = initial;
}