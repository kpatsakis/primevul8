_gdm_display_set_id (GdmDisplay     *self,
                     const char     *id)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        g_debug ("GdmDisplay: id: %s", id);
        g_free (priv->id);
        priv->id = g_strdup (id);
}