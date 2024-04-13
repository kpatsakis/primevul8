lzh_emit_window(struct lzh_stream *strm, size_t s)
{
	strm->ref_ptr = strm->ds->w_buff;
	strm->avail_out = (int)s;
	strm->total_out += s;
}