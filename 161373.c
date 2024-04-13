_gdm_display_set_seat_id (GdmDisplay     *self,
                          const char     *seat_id)
{
        GdmDisplayPrivate *priv;

        priv = gdm_display_get_instance_private (self);
        g_debug ("GdmDisplay: seat id: %s", seat_id);
        g_free (priv->seat_id);
        priv->seat_id = g_strdup (seat_id);
}