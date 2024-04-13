ncp__vol2io(struct ncp_server *server, unsigned char *iname, unsigned int *ilen,
		const unsigned char *vname, unsigned int vlen, int cc)
{
	struct nls_table *in = server->nls_vol;
	struct nls_table *out = server->nls_io;
	const unsigned char *vname_end;
	unsigned char *iname_start;
	unsigned char *iname_end;
	unsigned char *vname_cc;
	int err;

	vname_cc = NULL;

	if (cc) {
		int i;

		/* this is wrong! */
		vname_cc = kmalloc(vlen, GFP_KERNEL);
		if (!vname_cc)
			return -ENOMEM;
		for (i = 0; i < vlen; i++)
			vname_cc[i] = ncp_tolower(in, vname[i]);
		vname = vname_cc;
	}

	iname_start = iname;
	iname_end = iname + *ilen - 1;
	vname_end = vname + vlen;

	while (vname < vname_end) {
		wchar_t ec;
		int chl;

		if ( (chl = in->char2uni(vname, vname_end - vname, &ec)) < 0) {
			err = chl;
			goto quit;
		}
		vname += chl;

		/* unitolower should be here! */

		if (NCP_IS_FLAG(server, NCP_FLAG_UTF8)) {
			int k;

			k = utf32_to_utf8(ec, iname, iname_end - iname);
			if (k < 0) {
				err = -ENAMETOOLONG;
				goto quit;
			}
			iname += k;
		} else {
			if ( (chl = out->uni2char(ec, iname, iname_end - iname)) >= 0) {
				iname += chl;
			} else {
				int k;

				if (iname_end - iname < 5) {
					err = -ENAMETOOLONG;
					goto quit;
				}
				*iname = NCP_ESC;
				for (k = 4; k > 0; k--) {
					unsigned char v;
					
					v = (ec & 0xF) + '0';
					if (v > '9') {
						v += 'A' - '9' - 1;
					}
					iname[k] = v;
					ec >>= 4;
				}
				iname += 5;
			}
		}
	}

	*iname = 0;
	*ilen = iname - iname_start;
	err = 0;
quit:;
	if (cc)
		kfree(vname_cc);
	return err;
}