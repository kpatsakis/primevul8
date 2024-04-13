dissect_kerberos_encrypted_message(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, gssapi_encrypt_info_t* encrypt)
{
    tvbuff_t *kerberos_tvb;
    gint offset = 0, len;
    guint8 *data;

    proto_tree_add_item(tree, hf_multipart_sec_token_len, tvb, offset, 4, ENC_LITTLE_ENDIAN);
    offset += 4;
    len = tvb_reported_length_remaining(tvb, offset);

    DISSECTOR_ASSERT(tvb_bytes_exist(tvb, offset, len));

    data = (guint8 *)tvb_memdup(pinfo->pool, tvb, offset, len);
    kerberos_tvb = tvb_new_child_real_data(tvb, data, len, len);

    add_new_data_source(pinfo, kerberos_tvb, "Kerberos Data");
    call_dissector_with_data(gssapi_handle, kerberos_tvb, pinfo, tree, encrypt);
}