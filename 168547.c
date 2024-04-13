ncp__io2vol(struct ncp_server *server, unsigned char *vname, unsigned int *vlen,
		const unsigned char *iname, unsigned int ilen, int cc)
{
	struct nls_table *in = server->nls_io;
	struct nls_table *out = server->nls_vol;
	unsigned char *vname_start;
	unsigned char *vname_end;
	const unsigned char *iname_end;

	iname_end = iname + ilen;
	vname_start = vname;
	vname_end = vname + *vlen - 1;

	while (iname < iname_end) {
		int chl;
		wchar_t ec;

		if (NCP_IS_FLAG(server, NCP_FLAG_UTF8)) {
			int k;
			unicode_t u;

			k = utf8_to_utf32(iname, iname_end - iname, &u);
			if (k < 0 || u > MAX_WCHAR_T)
				return -EINVAL;
			iname += k;
			ec = u;
		} else {
			if (*iname == NCP_ESC) {
				int k;

				if (iname_end - iname < 5)
					goto nospec;

				ec = 0;
				for (k = 1; k < 5; k++) {
					unsigned char nc;

					nc = iname[k] - '0';
					if (nc >= 10) {
						nc -= 'A' - '0' - 10;
						if ((nc < 10) || (nc > 15)) {
							goto nospec;
						}
					}
					ec = (ec << 4) | nc;
				}
				iname += 5;
			} else {
nospec:;			
				if ( (chl = in->char2uni(iname, iname_end - iname, &ec)) < 0)
					return chl;
				iname += chl;
			}
		}

		/* unitoupper should be here! */

		chl = out->uni2char(ec, vname, vname_end - vname);
		if (chl < 0)
			return chl;

		/* this is wrong... */
		if (cc) {
			int chi;

			for (chi = 0; chi < chl; chi++){
				vname[chi] = ncp_toupper(out, vname[chi]);
			}
		}
		vname += chl;
	}

	*vname = 0;
	*vlen = vname - vname_start;
	return 0;
}