gdm_display_constructor (GType                  type,
                         guint                  n_construct_properties,
                         GObjectConstructParam *construct_properties)
{
        GdmDisplay        *self;
        GdmDisplayPrivate *priv;
        gboolean           res;

        self = GDM_DISPLAY (G_OBJECT_CLASS (gdm_display_parent_class)->constructor (type,
                                                                                    n_construct_properties,
                                                                                    construct_properties));

        priv = gdm_display_get_instance_private (self);

        g_free (priv->id);
        priv->id = g_strdup_printf ("/org/gnome/DisplayManager/Displays/%lu",
                                          (gulong) self);

        res = register_display (self);
        if (! res) {
                g_warning ("Unable to register display with system bus");
        }

        return G_OBJECT (self);
}