mm_answer_keyverify(int socket, Buffer *m)
{
	Key *key;
	u_char *signature, *data, *blob;
	u_int signaturelen, datalen, bloblen;
	int verified = 0;
	int valid_data = 0;

	blob = buffer_get_string(m, &bloblen);
	signature = buffer_get_string(m, &signaturelen);
	data = buffer_get_string(m, &datalen);

	if (hostbased_cuser == NULL || hostbased_chost == NULL ||
	  !monitor_allowed_key(blob, bloblen))
		fatal("%s: bad key, not previously allowed", __func__);

	key = key_from_blob(blob, bloblen);
	if (key == NULL)
		fatal("%s: bad public key blob", __func__);

	switch (key_blobtype) {
	case MM_USERKEY:
		valid_data = monitor_valid_userblob(data, datalen);
		break;
	case MM_HOSTKEY:
		valid_data = monitor_valid_hostbasedblob(data, datalen,
		    hostbased_cuser, hostbased_chost);
		break;
	default:
		valid_data = 0;
		break;
	}
	if (!valid_data)
		fatal("%s: bad signature data blob", __func__);

	verified = key_verify(key, signature, signaturelen, data, datalen);
	debug3("%s: key %p signature %s",
	    __func__, key, verified ? "verified" : "unverified");

	key_free(key);
	xfree(blob);
	xfree(signature);
	xfree(data);

	auth_method = key_blobtype == MM_USERKEY ? "publickey" : "hostbased";

	monitor_reset_key_state();

	buffer_clear(m);
	buffer_put_int(m, verified);
	mm_request_send(socket, MONITOR_ANS_KEYVERIFY, m);

	return (verified);
}