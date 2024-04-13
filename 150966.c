free_form_auth_data_list (gpointer data)
{
  GSList *p, *l = (GSList*)data;

  for (p = l; p; p = p->next)
    form_auth_data_free ((EphyEmbedSingleFormAuthData*)p->data);

  g_slist_free (l);
}