int jpc_putms(jas_stream_t *out, jpc_cstate_t *cstate, jpc_ms_t *ms)
{
	jas_stream_t *tmpstream;
	int len;

	/* Output the marker segment type. */
	if (jpc_putuint16(out, ms->id)) {
		return -1;
	}

	/* Output the marker segment length and parameters if necessary. */
	if (ms->ops->putparms) {
		/* Create a temporary stream in which to buffer the
		  parameter data. */
		if (!(tmpstream = jas_stream_memopen(0, 0))) {
			return -1;
		}
		if ((*ms->ops->putparms)(ms, cstate, tmpstream)) {
			jas_stream_close(tmpstream);
			return -1;
		}
		/* Get the number of bytes of parameter data written. */
		if ((len = jas_stream_tell(tmpstream)) < 0) {
			jas_stream_close(tmpstream);
			return -1;
		}
		ms->len = len;
		/* Write the marker segment length and parameter data to
		  the output stream. */
		if (jas_stream_seek(tmpstream, 0, SEEK_SET) < 0 ||
		  jpc_putuint16(out, ms->len + 2) ||
		  jas_stream_copy(out, tmpstream, ms->len) < 0) {
			jas_stream_close(tmpstream);
			return -1;
		}
		/* Close the temporary stream. */
		jas_stream_close(tmpstream);
	}

	/* This is a bit of a hack, but I'm not going to define another
	  type of virtual function for this one special case. */
	if (ms->id == JPC_MS_SIZ) {
		cstate->numcomps = ms->parms.siz.numcomps;
	}

	if (jas_getdbglevel() > 0) {
		jpc_ms_dump(ms, stderr);
	}

	return 0;
}