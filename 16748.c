save_dialog (void)
{
  GtkWidget *dialog;
  GtkWidget *table;
  GtkWidget *entry;
  GtkWidget *spinbutton;
  GtkObject *adj;
  gboolean   run;

  dialog = gimp_export_dialog_new (_("Brush"), PLUG_IN_BINARY, SAVE_PROC);

  /* The main table */
  table = gtk_table_new (2, 2, FALSE);
  gtk_container_set_border_width (GTK_CONTAINER (table), 12);
  gtk_table_set_row_spacings (GTK_TABLE (table), 6);
  gtk_table_set_col_spacings (GTK_TABLE (table), 6);
  gtk_box_pack_start (GTK_BOX (gimp_export_dialog_get_content_area (dialog)),
                      table, TRUE, TRUE, 0);
  gtk_widget_show (table);

  spinbutton = gimp_spin_button_new (&adj,
                                     info.spacing, 1, 1000, 1, 10, 0, 1, 0);
  gtk_entry_set_activates_default (GTK_ENTRY (spinbutton), TRUE);
  gimp_table_attach_aligned (GTK_TABLE (table), 0, 0,
                             _("Spacing:"), 1.0, 0.5,
                             spinbutton, 1, TRUE);

  g_signal_connect (adj, "value-changed",
                    G_CALLBACK (gimp_int_adjustment_update),
                    &info.spacing);

  entry = gtk_entry_new ();
  gtk_widget_set_size_request (entry, 200, -1);
  gtk_entry_set_text (GTK_ENTRY (entry), info.description);
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  gimp_table_attach_aligned (GTK_TABLE (table), 0, 1,
                             _("Description:"), 1.0, 0.5,
                             entry, 1, FALSE);

  g_signal_connect (entry, "changed",
                    G_CALLBACK (entry_callback),
                    info.description);

  gtk_widget_show (dialog);

  run = (gimp_dialog_run (GIMP_DIALOG (dialog)) == GTK_RESPONSE_OK);

  gtk_widget_destroy (dialog);

  return run;
}