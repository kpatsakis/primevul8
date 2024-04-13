mm_newkeys_to_blob(int mode, u_char **blobp, u_int *lenp)
{
	Buffer b;
	int len;
	Enc *enc;
	Mac *mac;
	Comp *comp;
	Newkeys *newkey = newkeys[mode];

	debug3("%s: converting %p", __func__, newkey);

	if (newkey == NULL) {
		error("%s: newkey == NULL", __func__);
		return 0;
	}
	enc = &newkey->enc;
	mac = &newkey->mac;
	comp = &newkey->comp;

	buffer_init(&b);
	/* Enc structure */
	buffer_put_cstring(&b, enc->name);
	/* The cipher struct is constant and shared, you export pointer */
	buffer_append(&b, &enc->cipher, sizeof(enc->cipher));
	buffer_put_int(&b, enc->enabled);
	buffer_put_int(&b, enc->block_size);
	buffer_put_string(&b, enc->key, enc->key_len);
	packet_get_keyiv(mode, enc->iv, enc->block_size);
	buffer_put_string(&b, enc->iv, enc->block_size);

	/* Mac structure */
	buffer_put_cstring(&b, mac->name);
	buffer_put_int(&b, mac->enabled);
	buffer_put_string(&b, mac->key, mac->key_len);

	/* Comp structure */
	buffer_put_int(&b, comp->type);
	buffer_put_int(&b, comp->enabled);
	buffer_put_cstring(&b, comp->name);

	len = buffer_len(&b);
	if (lenp != NULL)
		*lenp = len;
	if (blobp != NULL) {
		*blobp = xmalloc(len);
		memcpy(*blobp, buffer_ptr(&b), len);
	}
	memset(buffer_ptr(&b), 0, len);
	buffer_free(&b);
	return len;
}