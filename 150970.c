cache_keyring_form_data (EphyEmbedSingle *single)
{
  gnome_keyring_list_item_ids (GNOME_KEYRING_DEFAULT,
                               (GnomeKeyringOperationGetListCallback)store_form_data_cb,
                               single,
                               NULL);
}