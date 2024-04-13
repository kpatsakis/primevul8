base64_decode(packet_info *pinfo, tvbuff_t *b64_tvb, char *name)
{
    char *data;
    tvbuff_t *tvb;
    data = tvb_get_string_enc(wmem_packet_scope(), b64_tvb, 0, tvb_reported_length(b64_tvb), ENC_ASCII);

    tvb = base64_to_tvb(b64_tvb, data);
    add_new_data_source(pinfo, tvb, name);

    return tvb;
}