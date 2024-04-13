gpg_id_to_hash (CamelCipherContext *context,
                const gchar *id)
{
	if (id) {
		if (!strcmp (id, "pgp-md2"))
			return CAMEL_CIPHER_HASH_MD2;
		else if (!strcmp (id, "pgp-md5"))
			return CAMEL_CIPHER_HASH_MD5;
		else if (!strcmp (id, "pgp-sha1"))
			return CAMEL_CIPHER_HASH_SHA1;
		else if (!strcmp (id, "pgp-sha256"))
			return CAMEL_CIPHER_HASH_SHA256;
		else if (!strcmp (id, "pgp-sha384"))
			return CAMEL_CIPHER_HASH_SHA384;
		else if (!strcmp (id, "pgp-sha512"))
			return CAMEL_CIPHER_HASH_SHA512;
		else if (!strcmp (id, "pgp-ripemd160"))
			return CAMEL_CIPHER_HASH_RIPEMD160;
		else if (!strcmp (id, "tiger192"))
			return CAMEL_CIPHER_HASH_TIGER192;
		else if (!strcmp (id, "haval-5-160"))
			return CAMEL_CIPHER_HASH_HAVAL5160;
	}

	return CAMEL_CIPHER_HASH_DEFAULT;
}