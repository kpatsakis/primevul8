store_form_data_cb (GnomeKeyringResult result, GList *l, EphyEmbedSingle *single)
{
  GList *p;

  if (result != GNOME_KEYRING_RESULT_OK)
    return;

  for (p = l; p; p = p->next) {
    guint key_id = GPOINTER_TO_UINT (p->data);
    gnome_keyring_item_get_attributes (GNOME_KEYRING_DEFAULT,
                                       key_id,
                                       (GnomeKeyringOperationGetAttributesCallback) get_attr_cb,
                                       single,
                                       NULL);
  }
}