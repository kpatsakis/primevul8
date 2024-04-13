static void http_debug_hdr(const char *dir, struct stream *s, const struct ist n, const struct ist v)
{
        struct session *sess = strm_sess(s);
        int max;

        chunk_printf(&trash, "%08x:%s.%s[%04x:%04x]: ", s->uniq_id, s->be->id,
                     dir,
                     objt_conn(sess->origin) ? (unsigned short)__objt_conn(sess->origin)->handle.fd : -1,
                     objt_cs(s->si[1].end) ? (unsigned short)__objt_cs(s->si[1].end)->conn->handle.fd : -1);

        max = n.len;
        UBOUND(max, trash.size - trash.data - 3);
        chunk_memcat(&trash, n.ptr, max);
        trash.area[trash.data++] = ':';
        trash.area[trash.data++] = ' ';

        max = v.len;
        UBOUND(max, trash.size - trash.data - 1);
        chunk_memcat(&trash, v.ptr, max);
        trash.area[trash.data++] = '\n';

        DISGUISE(write(1, trash.area, trash.data));
}