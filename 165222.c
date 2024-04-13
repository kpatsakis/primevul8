gs_window_init (GSWindow *window)
{       
        window->priv = GS_WINDOW_GET_PRIVATE (window);

        window->priv->geometry.x      = -1;
        window->priv->geometry.y      = -1;
        window->priv->geometry.width  = -1;
        window->priv->geometry.height = -1;

        window->priv->last_x = -1;
        window->priv->last_y = -1;

        gtk_window_set_decorated (GTK_WINDOW (window), FALSE);

        gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), TRUE);
        gtk_window_set_skip_pager_hint (GTK_WINDOW (window), TRUE);

        gtk_window_set_keep_above (GTK_WINDOW (window), TRUE);

        gtk_window_fullscreen (GTK_WINDOW (window));

        gtk_widget_set_events (GTK_WIDGET (window),
                               gtk_widget_get_events (GTK_WIDGET (window))
                               | GDK_POINTER_MOTION_MASK
                               | GDK_BUTTON_PRESS_MASK
                               | GDK_BUTTON_RELEASE_MASK
                               | GDK_KEY_PRESS_MASK
                               | GDK_KEY_RELEASE_MASK
                               | GDK_EXPOSURE_MASK
                               | GDK_VISIBILITY_NOTIFY_MASK
                               | GDK_ENTER_NOTIFY_MASK
                               | GDK_LEAVE_NOTIFY_MASK);
                           
        GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

        GtkWidget *grid = gtk_grid_new();

        GdkColor transparentColor = { 0, 0, 0, 0 };

        gtk_widget_override_background_color(grid, GTK_STATE_NORMAL, &transparentColor);
        gtk_widget_show (grid);
                     
        window->priv->vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_show (window->priv->vbox);
        
        gtk_container_add (GTK_CONTAINER (window), main_box);                
        
        gtk_box_pack_start (GTK_BOX (main_box), grid, TRUE, TRUE, 0);
        gtk_container_set_border_width (GTK_CONTAINER (grid), 6);

        gtk_widget_show (main_box);
                                
        gtk_widget_set_valign (window->priv->vbox, GTK_ALIGN_CENTER);
        gtk_widget_set_halign (window->priv->vbox, GTK_ALIGN_CENTER);
        gtk_widget_set_size_request(window->priv->vbox,450, -1);
        
        // Default message        
        window->priv->default_message = g_settings_get_string(g_settings_new ("org.cinnamon.desktop.screensaver"), "default-message");
                
        // Clock -- need to find a way to make it appear on the bottom-left side of the background without shifting the position of the main dialog box
        window->priv->clock = gtk_label_new (NULL);
        gtk_widget_show (window->priv->clock);
        window->priv->clock_tracker = g_object_new (GNOME_TYPE_WALL_CLOCK, NULL);
        g_signal_connect (window->priv->clock_tracker, "notify::clock", G_CALLBACK (on_clock_changed), window);
        update_clock (window);

        gtk_misc_set_padding (GTK_MISC (window->priv->clock), 4, 4);
        gtk_grid_attach(GTK_GRID(grid), window->priv->clock, 0, 1, 1, 1);   
        gtk_grid_attach(GTK_GRID(grid), window->priv->vbox, 1, 1, 1, 1);        
        GtkWidget * right_label = gtk_label_new(NULL);        
        gtk_widget_show (right_label);
        gtk_grid_attach(GTK_GRID(grid), right_label, 2, 1, 1, 1); 
                
        gtk_grid_set_column_homogeneous (GTK_GRID(grid), TRUE);
        gtk_grid_set_column_spacing (GTK_GRID(grid), 6);
        
        gtk_widget_set_valign (grid, GTK_ALIGN_CENTER);
        gtk_widget_set_halign (grid, GTK_ALIGN_CENTER);
        gtk_widget_set_hexpand (grid, TRUE);
        gtk_widget_set_vexpand (grid, TRUE);

        g_signal_connect (main_box, "draw", G_CALLBACK (shade_background), window);

        create_info_bar (window);       
}