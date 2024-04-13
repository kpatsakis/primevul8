static void http_debug_stline(const char *dir, struct stream *s, const struct htx_sl *sl)
{
        struct session *sess = strm_sess(s);
        int max;

        chunk_printf(&trash, "%08x:%s.%s[%04x:%04x]: ", s->uniq_id, s->be->id,
                     dir,
                     objt_conn(sess->origin) ? (unsigned short)__objt_conn(sess->origin)->handle.fd : -1,
                     objt_cs(s->si[1].end) ? (unsigned short)__objt_cs(s->si[1].end)->conn->handle.fd : -1);

        max = HTX_SL_P1_LEN(sl);
        UBOUND(max, trash.size - trash.data - 3);
        chunk_memcat(&trash, HTX_SL_P1_PTR(sl), max);
        trash.area[trash.data++] = ' ';

        max = HTX_SL_P2_LEN(sl);
        UBOUND(max, trash.size - trash.data - 2);
        chunk_memcat(&trash, HTX_SL_P2_PTR(sl), max);
        trash.area[trash.data++] = ' ';

        max = HTX_SL_P3_LEN(sl);
        UBOUND(max, trash.size - trash.data - 1);
        chunk_memcat(&trash, HTX_SL_P3_PTR(sl), max);
        trash.area[trash.data++] = '\n';

        DISGUISE(write(1, trash.area, trash.data));
}