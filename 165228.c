set_info_text_and_icon (GSWindow   *window,
                        const char *icon_stock_id,
                        const char *primary_text,
                        const char *secondary_text)
{
        GtkWidget *content_area;
        GtkWidget *hbox_content;
        GtkWidget *image;
        GtkWidget *vbox;
        gchar *primary_markup;
        gchar *secondary_markup;
        GtkWidget *primary_label;
        GtkWidget *secondary_label;

        hbox_content = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);
        gtk_widget_show (hbox_content);

        image = gtk_image_new_from_stock (icon_stock_id, GTK_ICON_SIZE_DIALOG);
        gtk_widget_show (image);
        gtk_box_pack_start (GTK_BOX (hbox_content), image, FALSE, FALSE, 0);
        gtk_misc_set_alignment (GTK_MISC (image), 0.5, 0);

        vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
        gtk_widget_show (vbox);
        gtk_box_pack_start (GTK_BOX (hbox_content), vbox, FALSE, FALSE, 0);

        primary_markup = g_strdup_printf ("<b>%s</b>", primary_text);
        primary_label = gtk_label_new (primary_markup);
        g_free (primary_markup);
        gtk_widget_show (primary_label);
        gtk_box_pack_start (GTK_BOX (vbox), primary_label, TRUE, TRUE, 0);
        gtk_label_set_use_markup (GTK_LABEL (primary_label), TRUE);
        gtk_label_set_line_wrap (GTK_LABEL (primary_label), TRUE);
        gtk_misc_set_alignment (GTK_MISC (primary_label), 0, 0.5);

        if (secondary_text != NULL) {
                secondary_markup = g_strdup_printf ("<small>%s</small>",
                                                    secondary_text);
                secondary_label = gtk_label_new (secondary_markup);
                g_free (secondary_markup);
                gtk_widget_show (secondary_label);
                gtk_box_pack_start (GTK_BOX (vbox), secondary_label, TRUE, TRUE, 0);
                gtk_label_set_use_markup (GTK_LABEL (secondary_label), TRUE);
                gtk_label_set_line_wrap (GTK_LABEL (secondary_label), TRUE);
                gtk_misc_set_alignment (GTK_MISC (secondary_label), 0, 0.5);
        }

        /* remove old content */
        content_area = gtk_info_bar_get_content_area (GTK_INFO_BAR (window->priv->info_bar));
        if (window->priv->info_content != NULL) {
                gtk_container_remove (GTK_CONTAINER (content_area), window->priv->info_content);
        }
        gtk_box_pack_start (GTK_BOX (content_area),
                            hbox_content,
                            TRUE, FALSE, 0);
        window->priv->info_content = hbox_content;
}