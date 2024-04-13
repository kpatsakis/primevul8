client_use_key_share(gnutls_session_t session, const gnutls_group_entry_st *group,
		     const uint8_t * data, size_t data_size)
{
	const gnutls_ecc_curve_entry_st *curve;
	int ret;

	if (group->pk == GNUTLS_PK_EC) {
		gnutls_pk_params_st pub;

		curve = _gnutls_ecc_curve_get_params(group->curve);

		gnutls_pk_params_init(&pub);

		if (session->key.kshare.ecdh_params.algo != group->pk || session->key.kshare.ecdh_params.curve != curve->id)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		if (curve->size*2+1 != data_size)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		/* read the server's public key */
		ret = _gnutls_ecc_ansi_x962_import(data, data_size,
						   &pub.params[ECC_X],
						   &pub.params[ECC_Y]);
		if (ret < 0)
			return gnutls_assert_val(ret);

		pub.algo = group->pk;
		pub.curve = curve->id;
		pub.params_nr = 2;

		/* generate shared key */
		ret = _gnutls_pk_derive_tls13(curve->pk, &session->key.key, &session->key.kshare.ecdh_params, &pub);
		gnutls_pk_params_release(&pub);
		if (ret < 0) {
			return gnutls_assert_val(ret);
		}

		ret = 0;

	} else if (group->pk == GNUTLS_PK_ECDH_X25519 ||
		   group->pk == GNUTLS_PK_ECDH_X448) {
		gnutls_pk_params_st pub;

		curve = _gnutls_ecc_curve_get_params(group->curve);

		if (session->key.kshare.ecdhx_params.algo != group->pk || session->key.kshare.ecdhx_params.curve != curve->id)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		if (curve->size != data_size)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		/* read the public key and generate shared */
		gnutls_pk_params_init(&pub);

		pub.algo = group->pk;
		pub.curve = curve->id;

		pub.raw_pub.data = (void*)data;
		pub.raw_pub.size = data_size;

		/* We don't mask the MSB in the final byte as required
		 * by RFC7748. This will be done internally by nettle 3.3 or later.
		 */
		ret = _gnutls_pk_derive_tls13(curve->pk, &session->key.key, &session->key.kshare.ecdhx_params, &pub);
		if (ret < 0) {
			return gnutls_assert_val(ret);
		}

		ret = 0;

	} else if (group->pk == GNUTLS_PK_DH) {
		gnutls_pk_params_st pub;

		if (session->key.kshare.dh_params.algo != group->pk || session->key.kshare.dh_params.dh_group != group->id)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		if (data_size != group->prime->size)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		/* read the public key and generate shared */
		gnutls_pk_params_init(&pub);

		ret = _gnutls_mpi_init_scan_nz(&pub.params[DH_Y],
			data, data_size);
		if (ret < 0)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		pub.algo = group->pk;

		/* generate shared key */
		ret = _gnutls_pk_derive_tls13(GNUTLS_PK_DH, &session->key.key, &session->key.kshare.dh_params, &pub);
		_gnutls_mpi_release(&pub.params[DH_Y]);
		if (ret < 0)
			return gnutls_assert_val(ret);

		ret = 0;
	} else {
		return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);
	}

	_gnutls_debug_log("EXT[%p]: client generated %s shared key\n", session, group->name);

	return ret;
}