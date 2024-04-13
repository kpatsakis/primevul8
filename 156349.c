mm_newkeys_from_blob(u_char *blob, int blen)
{
	Buffer b;
	u_int len;
	Newkeys *newkey = NULL;
	Enc *enc;
	Mac *mac;
	Comp *comp;

	debug3("%s: %p(%d)", __func__, blob, blen);
#ifdef DEBUG_PK
	dump_base64(stderr, blob, blen);
#endif
	buffer_init(&b);
	buffer_append(&b, blob, blen);

	newkey = xmalloc(sizeof(*newkey));
	enc = &newkey->enc;
	mac = &newkey->mac;
	comp = &newkey->comp;

	/* Enc structure */
	enc->name = buffer_get_string(&b, NULL);
	buffer_get(&b, &enc->cipher, sizeof(enc->cipher));
	enc->enabled = buffer_get_int(&b);
	enc->block_size = buffer_get_int(&b);
	enc->key = buffer_get_string(&b, &enc->key_len);
	enc->iv = buffer_get_string(&b, &len);
	if (len != enc->block_size)
		fatal("%s: bad ivlen: expected %u != %u", __func__,
		    enc->block_size, len);

	if (enc->name == NULL || cipher_by_name(enc->name) != enc->cipher)
		fatal("%s: bad cipher name %s or pointer %p", __func__,
		    enc->name, enc->cipher);

	/* Mac structure */
	mac->name = buffer_get_string(&b, NULL);
	if (mac->name == NULL || mac_init(mac, mac->name) == -1)
		fatal("%s: can not init mac %s", __func__, mac->name);
	mac->enabled = buffer_get_int(&b);
	mac->key = buffer_get_string(&b, &len);
	if (len > mac->key_len)
		fatal("%s: bad mac key length: %u > %d", __func__, len,
		    mac->key_len);
	mac->key_len = len;

	/* Comp structure */
	comp->type = buffer_get_int(&b);
	comp->enabled = buffer_get_int(&b);
	comp->name = buffer_get_string(&b, NULL);

	len = buffer_len(&b);
	if (len != 0)
		error("newkeys_from_blob: remaining bytes in blob %u", len);
	buffer_free(&b);
	return (newkey);
}