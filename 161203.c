set_file_identifier(unsigned char *bp, int from, int to, enum vdc vdc,
    struct archive_write *a, struct vdd *vdd, struct archive_string *id,
    const char *label, int leading_under, enum char_type char_type)
{
	char identifier[256];
	struct isoent *isoent;
	const char *ids;
	size_t len;
	int r;

	if (id->length > 0 && leading_under && id->s[0] != '_') {
		if (char_type == A_CHAR)
			r = set_str_a_characters_bp(a, bp, from, to, id->s, vdc);
		else
			r = set_str_d_characters_bp(a, bp, from, to, id->s, vdc);
	} else if (id->length > 0) {
		ids = id->s;
		if (leading_under)
			ids++;
		isoent = isoent_find_entry(vdd->rootent, ids);
		if (isoent == NULL) {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Not Found %s `%s'.",
			    label, ids);
			return (ARCHIVE_FATAL);
		}
		len = isoent->ext_off + isoent->ext_len;
		if (vdd->vdd_type == VDD_JOLIET) {
			if (len > sizeof(identifier)-2)
				len = sizeof(identifier)-2;
		} else {
			if (len > sizeof(identifier)-1)
				len = sizeof(identifier)-1;
		}
		memcpy(identifier, isoent->identifier, len);
		identifier[len] = '\0';
		if (vdd->vdd_type == VDD_JOLIET) {
			identifier[len+1] = 0;
			vdc = VDC_UCS2_DIRECT;
		}
		if (char_type == A_CHAR)
			r = set_str_a_characters_bp(a, bp, from, to,
			    identifier, vdc);
		else
			r = set_str_d_characters_bp(a, bp, from, to,
			    identifier, vdc);
	} else {
		if (char_type == A_CHAR)
			r = set_str_a_characters_bp(a, bp, from, to, NULL, vdc);
		else
			r = set_str_d_characters_bp(a, bp, from, to, NULL, vdc);
	}
	return (r);
}