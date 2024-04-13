static size_t http_fmt_req_line(const struct htx_sl *sl, char *str, size_t len)
{
	struct ist dst = ist2(str, 0);

	if (istcat(&dst, htx_sl_req_meth(sl), len) == -1)
		goto end;
	if (dst.len + 1 > len)
		goto end;
	dst.ptr[dst.len++] = ' ';

	if (istcat(&dst, htx_sl_req_uri(sl), len) == -1)
		goto end;
	if (dst.len + 1 > len)
		goto end;
	dst.ptr[dst.len++] = ' ';

	istcat(&dst, htx_sl_req_vsn(sl), len);
  end:
	return dst.len;
}