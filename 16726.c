add_signers (CamelCipherValidity *validity,
             const GString *signers)
{
	CamelInternetAddress *address;
	gint i, count;

	g_return_if_fail (validity != NULL);

	if (!signers || !signers->str || !*signers->str)
		return;

	address = camel_internet_address_new ();
	g_return_if_fail (address != NULL);

	count = camel_address_decode (CAMEL_ADDRESS (address), signers->str);
	for (i = 0; i < count; i++) {
		const gchar *name = NULL, *email = NULL;

		if (!camel_internet_address_get (address, i, &name, &email))
			break;

		camel_cipher_validity_add_certinfo (validity, CAMEL_CIPHER_VALIDITY_SIGN, name, email);
	}

	g_object_unref (address);
}