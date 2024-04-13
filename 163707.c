void debug_hdr(const char *dir, struct session *t, const char *start, const char *end)
{
	int max;
	chunk_printf(&trash, "%08x:%s.%s[%04x:%04x]: ", t->uniq_id, t->be->id,
		      dir, (unsigned  short)t->req->prod->conn->t.sock.fd,
		     (unsigned short)t->req->cons->conn->t.sock.fd);

	for (max = 0; start + max < end; max++)
		if (start[max] == '\r' || start[max] == '\n')
			break;

	UBOUND(max, trash.size - trash.len - 3);
	trash.len += strlcpy2(trash.str + trash.len, start, max + 1);
	trash.str[trash.len++] = '\n';
	if (write(1, trash.str, trash.len) < 0) /* shut gcc warning */;
}