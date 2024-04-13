entry_callback (GtkWidget *widget,
                gpointer   data)
{
  strncpy (info.description, gtk_entry_get_text (GTK_ENTRY (widget)),
           sizeof (info.description));
  info.description[sizeof (info.description) - 1] = '\0';
}