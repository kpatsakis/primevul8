proto_tree_add_split_bits_crumb(proto_tree *tree, const int hfindex, tvbuff_t *tvb, const guint bit_offset,
				const crumb_spec_t *crumb_spec, guint16 crumb_index)
{
	header_field_info *hfinfo;

	PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo);
	proto_tree_add_text_internal(tree, tvb,
			    bit_offset >> 3,
			    ((bit_offset + crumb_spec[crumb_index].crumb_bit_length - 1) >> 3) - (bit_offset >> 3) + 1,
			    "%s crumb %d of %s (decoded above)",
			    decode_bits_in_field(bit_offset, crumb_spec[crumb_index].crumb_bit_length,
						 tvb_get_bits(tvb,
							      bit_offset,
							      crumb_spec[crumb_index].crumb_bit_length,
							      ENC_BIG_ENDIAN)),
			    crumb_index,
			    hfinfo->name);
}