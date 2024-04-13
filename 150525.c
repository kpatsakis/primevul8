server_use_key_share(gnutls_session_t session, const gnutls_group_entry_st *group,
		     const uint8_t * data, size_t data_size)
{
	const gnutls_ecc_curve_entry_st *curve;
	int ret;

	if (group->pk == GNUTLS_PK_EC) {
		gnutls_pk_params_st pub;

		gnutls_pk_params_release(&session->key.kshare.ecdh_params);
		gnutls_pk_params_init(&session->key.kshare.ecdh_params);

		curve = _gnutls_ecc_curve_get_params(group->curve);

		gnutls_pk_params_init(&pub);

		if (curve->size*2+1 != data_size)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		/* generate our key */
		ret = _gnutls_pk_generate_keys(curve->pk, curve->id, &session->key.kshare.ecdh_params, 1);
		if (ret < 0)
			return gnutls_assert_val(ret);

		/* read the public key */
		ret = _gnutls_ecc_ansi_x962_import(data, data_size,
						   &pub.params[ECC_X],
						   &pub.params[ECC_Y]);
		if (ret < 0)
			return gnutls_assert_val(ret);

		pub.algo = group->pk;
		pub.curve = curve->id;
		pub.params_nr = 2;

		/* generate shared */
		ret = _gnutls_pk_derive_tls13(curve->pk, &session->key.key, &session->key.kshare.ecdh_params, &pub);
		gnutls_pk_params_release(&pub);
		if (ret < 0) {
			return gnutls_assert_val(ret);
		}

		ret = 0;

	} else if (group->pk == GNUTLS_PK_ECDH_X25519 ||
		   group->pk == GNUTLS_PK_ECDH_X448) {
		gnutls_pk_params_st pub;

		gnutls_pk_params_release(&session->key.kshare.ecdhx_params);
		gnutls_pk_params_init(&session->key.kshare.ecdhx_params);

		curve = _gnutls_ecc_curve_get_params(group->curve);

		if (curve->size != data_size)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		/* generate our key */
		ret = _gnutls_pk_generate_keys(curve->pk, curve->id, &session->key.kshare.ecdhx_params, 1);
		if (ret < 0)
			return gnutls_assert_val(ret);

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

		/* we need to initialize the group parameters first */
		gnutls_pk_params_release(&session->key.kshare.dh_params);
		gnutls_pk_params_init(&session->key.kshare.dh_params);

		if (data_size != group->prime->size)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		/* set group params */
		ret = _gnutls_mpi_init_scan_nz(&session->key.kshare.dh_params.params[DH_G],
			group->generator->data, group->generator->size);
		if (ret < 0)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		ret = _gnutls_mpi_init_scan_nz(&session->key.kshare.dh_params.params[DH_P],
			group->prime->data, group->prime->size);
		if (ret < 0)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		ret = _gnutls_mpi_init_scan_nz(&session->key.kshare.dh_params.params[DH_Q],
			group->q->data, group->q->size);
		if (ret < 0)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		session->key.kshare.dh_params.algo = GNUTLS_PK_DH;
		session->key.kshare.dh_params.qbits = *group->q_bits;
		session->key.kshare.dh_params.params_nr = 3;

		/* generate our keys */
		ret = _gnutls_pk_generate_keys(group->pk, 0, &session->key.kshare.dh_params, 1);
		if (ret < 0)
			return gnutls_assert_val(ret);

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

	_gnutls_debug_log("EXT[%p]: server generated %s shared key\n", session, group->name);

	return ret;
}