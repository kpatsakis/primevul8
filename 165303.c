update_clock (GSWindow *window)
{	
        char *markup;

        if (window->priv->away_message != NULL && g_strcmp0(window->priv->away_message, "") != 0) {
                markup = g_strdup_printf ("%s\n\n<b><span font_desc=\"Ubuntu 14\" foreground=\"#CCCCCC\">%s</span></b>\n<b><span font_desc=\"Ubuntu 10\" foreground=\"#ACACAC\">  ~ %s</span></b>", gnome_wall_clock_get_clock (window->priv->clock_tracker), g_markup_escape_text(window->priv->away_message, -1), get_user_display_name());
        } else {
                markup = g_strdup_printf ("%s\n\n<b><span font_desc=\"Ubuntu 14\" foreground=\"#CCCCCC\">%s</span></b>", gnome_wall_clock_get_clock (window->priv->clock_tracker), g_markup_escape_text(window->priv->default_message, -1));
        }

        gtk_label_set_markup (GTK_LABEL (window->priv->clock), markup);
        gtk_label_set_line_wrap (GTK_LABEL (window->priv->clock), TRUE);
        gtk_misc_set_alignment (GTK_MISC (window->priv->clock), 0.5, 0.5);

        g_free (markup);
}