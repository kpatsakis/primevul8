gpg_hash_str (CamelCipherHash hash)
{
	switch (hash) {
	case CAMEL_CIPHER_HASH_MD2:
		return "--digest-algo=MD2";
	case CAMEL_CIPHER_HASH_MD5:
		return "--digest-algo=MD5";
	case CAMEL_CIPHER_HASH_SHA1:
		return "--digest-algo=SHA1";
	case CAMEL_CIPHER_HASH_SHA256:
		return "--digest-algo=SHA256";
	case CAMEL_CIPHER_HASH_SHA384:
		return "--digest-algo=SHA384";
	case CAMEL_CIPHER_HASH_SHA512:
		return "--digest-algo=SHA512";
	case CAMEL_CIPHER_HASH_RIPEMD160:
		return "--digest-algo=RIPEMD160";
	default:
		return NULL;
	}
}