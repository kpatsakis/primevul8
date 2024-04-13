static inline void map_to_unicode(unsigned code, const enc_to_uni *table, unsigned *res)
{
	/* only single byte encodings are currently supported; assumed code <= 0xFF */
	*res = table->inner[ENT_ENC_TO_UNI_STAGE1(code)]->uni_cp[ENT_ENC_TO_UNI_STAGE2(code)];
}