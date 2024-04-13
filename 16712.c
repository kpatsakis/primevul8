camel_gpg_context_class_init (CamelGpgContextClass *class)
{
	GObjectClass *object_class;
	CamelCipherContextClass *cipher_context_class;

	g_type_class_add_private (class, sizeof (CamelGpgContextPrivate));

	object_class = G_OBJECT_CLASS (class);
	object_class->set_property = gpg_context_set_property;
	object_class->get_property = gpg_context_get_property;

	cipher_context_class = CAMEL_CIPHER_CONTEXT_CLASS (class);
	cipher_context_class->sign_protocol = "application/pgp-signature";
	cipher_context_class->encrypt_protocol = "application/pgp-encrypted";
	cipher_context_class->key_protocol = "application/pgp-keys";
	cipher_context_class->hash_to_id = gpg_hash_to_id;
	cipher_context_class->id_to_hash = gpg_id_to_hash;
	cipher_context_class->sign_sync = gpg_sign_sync;
	cipher_context_class->verify_sync = gpg_verify_sync;
	cipher_context_class->encrypt_sync = gpg_encrypt_sync;
	cipher_context_class->decrypt_sync = gpg_decrypt_sync;
	cipher_context_class->import_keys_sync = gpg_import_keys_sync;
	cipher_context_class->export_keys_sync = gpg_export_keys_sync;

	g_object_class_install_property (
		object_class,
		PROP_ALWAYS_TRUST,
		g_param_spec_boolean (
			"always-trust",
			"Always Trust",
			NULL,
			FALSE,
			G_PARAM_READWRITE |
			G_PARAM_CONSTRUCT));
}