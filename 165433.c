static int jpc_siz_putparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *out)
{
	jpc_siz_t *siz = &ms->parms.siz;
	unsigned int i;

	/* Eliminate compiler warning about unused variables. */
	cstate = 0;

	assert(siz->width && siz->height && siz->tilewidth &&
	  siz->tileheight && siz->numcomps);
	if (jpc_putuint16(out, siz->caps) ||
	  jpc_putuint32(out, siz->width) ||
	  jpc_putuint32(out, siz->height) ||
	  jpc_putuint32(out, siz->xoff) ||
	  jpc_putuint32(out, siz->yoff) ||
	  jpc_putuint32(out, siz->tilewidth) ||
	  jpc_putuint32(out, siz->tileheight) ||
	  jpc_putuint32(out, siz->tilexoff) ||
	  jpc_putuint32(out, siz->tileyoff) ||
	  jpc_putuint16(out, siz->numcomps)) {
		return -1;
	}
	for (i = 0; i < siz->numcomps; ++i) {
		if (jpc_putuint8(out, ((siz->comps[i].sgnd & 1) << 7) |
		  ((siz->comps[i].prec - 1) & 0x7f)) ||
		  jpc_putuint8(out, siz->comps[i].hsamp) ||
		  jpc_putuint8(out, siz->comps[i].vsamp)) {
			return -1;
		}
	}
	return 0;
}