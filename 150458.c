static bool parse_signature(struct MACH0_(obj_t) *bin, ut64 off) {
	int i,len;
	ut32 data;
	bin->signature = NULL;
	struct linkedit_data_command link = {0};
	ut8 lit[sizeof (struct linkedit_data_command)] = {0};
	struct blob_index_t idx = {0};
	struct super_blob_t super = {{0}};

	if (off > bin->size || off + sizeof (struct linkedit_data_command) > bin->size) {
		return false;
	}
	len = r_buf_read_at (bin->b, off, lit, sizeof (struct linkedit_data_command));
	if (len != sizeof (struct linkedit_data_command)) {
		bprintf ("Failed to get data while parsing LC_CODE_SIGNATURE command\n");
		return false;
	}
	link.cmd = r_read_ble32 (&lit[0], bin->big_endian);
	link.cmdsize = r_read_ble32 (&lit[4], bin->big_endian);
	link.dataoff = r_read_ble32 (&lit[8], bin->big_endian);
	link.datasize = r_read_ble32 (&lit[12], bin->big_endian);

	data = link.dataoff;
	if (data > bin->size || data + sizeof (struct super_blob_t) > bin->size) {
		bin->signature = (ut8 *)strdup ("Malformed entitlement");
		return true;
	}
	super.blob.magic = r_buf_read_ble32_at (bin->b, data, mach0_endian);
	super.blob.length = r_buf_read_ble32_at (bin->b, data + 4, mach0_endian);
	super.count = r_buf_read_ble32_at (bin->b, data + 8, mach0_endian);
	char *verbose = r_sys_getenv ("RABIN2_CODESIGN_VERBOSE");
	bool isVerbose = false;
	if (verbose) {
		isVerbose = *verbose;
		free (verbose);
	}
	// to dump all certificates
	// [0x00053f75]> b 5K;/x 30800609;wtf @@ hit*
	// then do this:
	// $ openssl asn1parse -inform der -in a|less
	// $ openssl pkcs7 -inform DER -print_certs -text -in a
	for (i = 0; i < super.count; i++) {
		if (data + i > bin->size) {
			bin->signature = (ut8 *)strdup ("Malformed entitlement");
			break;
		}
		struct blob_index_t bi;
		if (r_buf_read_at (bin->b, data + 12 + (i * sizeof (struct blob_index_t)),
			(ut8*)&bi, sizeof (struct blob_index_t)) < sizeof (struct blob_index_t)) {
			break;
		}
		idx.type = r_read_ble32 (&bi.type, mach0_endian);
		idx.offset = r_read_ble32 (&bi.offset, mach0_endian);
		switch (idx.type) {
		case CSSLOT_ENTITLEMENTS:
			if (true || isVerbose) {
				ut64 off = data + idx.offset;
				if (off > bin->size || off + sizeof (struct blob_t) > bin->size) {
					bin->signature = (ut8 *)strdup ("Malformed entitlement");
					break;
				}
				struct blob_t entitlements = {0};
				entitlements.magic = r_buf_read_ble32_at (bin->b, off, mach0_endian);
				entitlements.length = r_buf_read_ble32_at (bin->b, off + 4, mach0_endian);
				len = entitlements.length - sizeof (struct blob_t);
				if (len <= bin->size && len > 1) {
					bin->signature = calloc (1, len + 1);
					if (!bin->signature) {
						break;
					}
					if (off + sizeof (struct blob_t) + len < r_buf_size (bin->b)) {
						r_buf_read_at (bin->b, off + sizeof (struct blob_t), (ut8 *)bin->signature, len);
						if (len >= 0) {
							bin->signature[len] = '\0';
						}
					} else {
						bin->signature = (ut8 *)strdup ("Malformed entitlement");
					}
				} else {
					bin->signature = (ut8 *)strdup ("Malformed entitlement");
				}
			}
			break;
		case CSSLOT_CODEDIRECTORY:
			if (isVerbose) {
				parseCodeDirectory (bin->b, data + idx.offset, link.datasize);
			}
			break;
		case 0x1000:
			// unknown
			break;
		case CSSLOT_CMS_SIGNATURE: // ASN1/DER certificate
			if (isVerbose) {
				ut8 header[8] = {0};
				r_buf_read_at (bin->b, data + idx.offset, header, sizeof (header));
				ut32 length = R_MIN (UT16_MAX, r_read_ble32 (header + 4, 1));
				ut8 *p = calloc (length, 1);
				if (p) {
					r_buf_read_at (bin->b, data + idx.offset + 0, p, length);
					ut32 *words = (ut32*)p;
					eprintf ("Magic: %x\n", words[0]);
					eprintf ("wtf DUMP @%d!%d\n",
						(int)data + idx.offset + 8, (int)length);
					eprintf ("openssl pkcs7 -print_certs -text -inform der -in DUMP\n");
					eprintf ("openssl asn1parse -offset %d -length %d -inform der -in /bin/ls\n",
						(int)data + idx.offset + 8, (int)length);
					eprintf ("pFp@%d!%d\n",
						(int)data + idx.offset + 8, (int)length);
					free (p);
				}
			}
			break;
		case CSSLOT_REQUIREMENTS: // 2
			{
				ut8 p[256];
				r_buf_read_at (bin->b, data + idx.offset + 16, p, sizeof (p));
				p[sizeof (p) - 1] = 0;
				ut32 slot_size = r_read_ble32 (p + 8, 1);
				if (slot_size < sizeof (p)) {
					ut32 ident_size = r_read_ble32 (p + 8, 1);
					if (!ident_size || ident_size > sizeof (p) - 28) {
						break;
					}
					char *ident = r_str_ndup ((const char *)p + 28, ident_size);
					if (ident) {
						sdb_set (bin->kv, "mach0.ident", ident, 0);
						free (ident);
					}
				} else {
					if (bin->verbose) {
						eprintf ("Invalid code slot size\n");
					}
				}
			}
			break;
		case CSSLOT_INFOSLOT: // 1;
		case CSSLOT_RESOURCEDIR: // 3;
		case CSSLOT_APPLICATION: // 4;
			// TODO: parse those codesign slots
			if (bin->verbose) {
				eprintf ("TODO: Some codesign slots are not yet supported\n");
			}
			break;
		default:
			if (bin->verbose) {
				eprintf ("Unknown Code signature slot %d\n", idx.type);
			}
			break;
		}
	}
	if (!bin->signature) {
		bin->signature = (ut8 *)strdup ("No entitlement found");
	}
	return true;
}