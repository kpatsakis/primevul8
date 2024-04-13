const char *http_parse_reqline(struct http_msg *msg,
			       unsigned int state, const char *ptr, const char *end,
			       unsigned int *ret_ptr, unsigned int *ret_state)
{
	const char *msg_start = msg->chn->buf->p;

	switch (state)	{
	case HTTP_MSG_RQMETH:
	http_msg_rqmeth:
		if (likely(HTTP_IS_TOKEN(*ptr)))
			EAT_AND_JUMP_OR_RETURN(http_msg_rqmeth, HTTP_MSG_RQMETH);

		if (likely(HTTP_IS_SPHT(*ptr))) {
			msg->sl.rq.m_l = ptr - msg_start;
			EAT_AND_JUMP_OR_RETURN(http_msg_rqmeth_sp, HTTP_MSG_RQMETH_SP);
		}

		if (likely(HTTP_IS_CRLF(*ptr))) {
			/* HTTP 0.9 request */
			msg->sl.rq.m_l = ptr - msg_start;
		http_msg_req09_uri:
			msg->sl.rq.u = ptr - msg_start;
		http_msg_req09_uri_e:
			msg->sl.rq.u_l = ptr - msg_start - msg->sl.rq.u;
		http_msg_req09_ver:
			msg->sl.rq.v = ptr - msg_start;
			msg->sl.rq.v_l = 0;
			goto http_msg_rqline_eol;
		}
		state = HTTP_MSG_ERROR;
		break;

	case HTTP_MSG_RQMETH_SP:
	http_msg_rqmeth_sp:
		if (likely(!HTTP_IS_LWS(*ptr))) {
			msg->sl.rq.u = ptr - msg_start;
			goto http_msg_rquri;
		}
		if (likely(HTTP_IS_SPHT(*ptr)))
			EAT_AND_JUMP_OR_RETURN(http_msg_rqmeth_sp, HTTP_MSG_RQMETH_SP);
		/* so it's a CR/LF, meaning an HTTP 0.9 request */
		goto http_msg_req09_uri;

	case HTTP_MSG_RQURI:
	http_msg_rquri:
		if (likely((unsigned char)(*ptr - 33) <= 93)) /* 33 to 126 included */
			EAT_AND_JUMP_OR_RETURN(http_msg_rquri, HTTP_MSG_RQURI);

		if (likely(HTTP_IS_SPHT(*ptr))) {
			msg->sl.rq.u_l = ptr - msg_start - msg->sl.rq.u;
			EAT_AND_JUMP_OR_RETURN(http_msg_rquri_sp, HTTP_MSG_RQURI_SP);
		}

		if (likely((unsigned char)*ptr >= 128)) {
			/* non-ASCII chars are forbidden unless option
			 * accept-invalid-http-request is enabled in the frontend.
			 * In any case, we capture the faulty char.
			 */
			if (msg->err_pos < -1)
				goto invalid_char;
			if (msg->err_pos == -1)
				msg->err_pos = ptr - msg_start;
			EAT_AND_JUMP_OR_RETURN(http_msg_rquri, HTTP_MSG_RQURI);
		}

		if (likely(HTTP_IS_CRLF(*ptr))) {
			/* so it's a CR/LF, meaning an HTTP 0.9 request */
			goto http_msg_req09_uri_e;
		}

		/* OK forbidden chars, 0..31 or 127 */
	invalid_char:
		msg->err_pos = ptr - msg_start;
		state = HTTP_MSG_ERROR;
		break;

	case HTTP_MSG_RQURI_SP:
	http_msg_rquri_sp:
		if (likely(!HTTP_IS_LWS(*ptr))) {
			msg->sl.rq.v = ptr - msg_start;
			goto http_msg_rqver;
		}
		if (likely(HTTP_IS_SPHT(*ptr)))
			EAT_AND_JUMP_OR_RETURN(http_msg_rquri_sp, HTTP_MSG_RQURI_SP);
		/* so it's a CR/LF, meaning an HTTP 0.9 request */
		goto http_msg_req09_ver;

	case HTTP_MSG_RQVER:
	http_msg_rqver:
		if (likely(HTTP_IS_VER_TOKEN(*ptr)))
			EAT_AND_JUMP_OR_RETURN(http_msg_rqver, HTTP_MSG_RQVER);

		if (likely(HTTP_IS_CRLF(*ptr))) {
			msg->sl.rq.v_l = ptr - msg_start - msg->sl.rq.v;
		http_msg_rqline_eol:
			/* We have seen the end of line. Note that we do not
			 * necessarily have the \n yet, but at least we know that we
			 * have EITHER \r OR \n, otherwise the request would not be
			 * complete. We can then record the request length and return
			 * to the caller which will be able to register it.
			 */
			msg->sl.rq.l = ptr - msg_start - msg->sol;
			return ptr;
		}

		/* neither an HTTP_VER token nor a CRLF */
		state = HTTP_MSG_ERROR;
		break;

#ifdef DEBUG_FULL
	default:
		fprintf(stderr, "FIXME !!!! impossible state at %s:%d = %d\n", __FILE__, __LINE__, state);
		exit(1);
#endif
	}

 http_msg_ood:
	/* out of valid data */
	if (ret_state)
		*ret_state = state;
	if (ret_ptr)
		*ret_ptr = ptr - msg_start;
	return NULL;
}