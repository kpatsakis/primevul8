const char *http_parse_stsline(struct http_msg *msg,
			       unsigned int state, const char *ptr, const char *end,
			       unsigned int *ret_ptr, unsigned int *ret_state)
{
	const char *msg_start = msg->chn->buf->p;

	switch (state)	{
	case HTTP_MSG_RPVER:
	http_msg_rpver:
		if (likely(HTTP_IS_VER_TOKEN(*ptr)))
			EAT_AND_JUMP_OR_RETURN(http_msg_rpver, HTTP_MSG_RPVER);

		if (likely(HTTP_IS_SPHT(*ptr))) {
			msg->sl.st.v_l = ptr - msg_start;
			EAT_AND_JUMP_OR_RETURN(http_msg_rpver_sp, HTTP_MSG_RPVER_SP);
		}
		state = HTTP_MSG_ERROR;
		break;

	case HTTP_MSG_RPVER_SP:
	http_msg_rpver_sp:
		if (likely(!HTTP_IS_LWS(*ptr))) {
			msg->sl.st.c = ptr - msg_start;
			goto http_msg_rpcode;
		}
		if (likely(HTTP_IS_SPHT(*ptr)))
			EAT_AND_JUMP_OR_RETURN(http_msg_rpver_sp, HTTP_MSG_RPVER_SP);
		/* so it's a CR/LF, this is invalid */
		state = HTTP_MSG_ERROR;
		break;

	case HTTP_MSG_RPCODE:
	http_msg_rpcode:
		if (likely(!HTTP_IS_LWS(*ptr)))
			EAT_AND_JUMP_OR_RETURN(http_msg_rpcode, HTTP_MSG_RPCODE);

		if (likely(HTTP_IS_SPHT(*ptr))) {
			msg->sl.st.c_l = ptr - msg_start - msg->sl.st.c;
			EAT_AND_JUMP_OR_RETURN(http_msg_rpcode_sp, HTTP_MSG_RPCODE_SP);
		}

		/* so it's a CR/LF, so there is no reason phrase */
		msg->sl.st.c_l = ptr - msg_start - msg->sl.st.c;
	http_msg_rsp_reason:
		/* FIXME: should we support HTTP responses without any reason phrase ? */
		msg->sl.st.r = ptr - msg_start;
		msg->sl.st.r_l = 0;
		goto http_msg_rpline_eol;

	case HTTP_MSG_RPCODE_SP:
	http_msg_rpcode_sp:
		if (likely(!HTTP_IS_LWS(*ptr))) {
			msg->sl.st.r = ptr - msg_start;
			goto http_msg_rpreason;
		}
		if (likely(HTTP_IS_SPHT(*ptr)))
			EAT_AND_JUMP_OR_RETURN(http_msg_rpcode_sp, HTTP_MSG_RPCODE_SP);
		/* so it's a CR/LF, so there is no reason phrase */
		goto http_msg_rsp_reason;

	case HTTP_MSG_RPREASON:
	http_msg_rpreason:
		if (likely(!HTTP_IS_CRLF(*ptr)))
			EAT_AND_JUMP_OR_RETURN(http_msg_rpreason, HTTP_MSG_RPREASON);
		msg->sl.st.r_l = ptr - msg_start - msg->sl.st.r;
	http_msg_rpline_eol:
		/* We have seen the end of line. Note that we do not
		 * necessarily have the \n yet, but at least we know that we
		 * have EITHER \r OR \n, otherwise the response would not be
		 * complete. We can then record the response length and return
		 * to the caller which will be able to register it.
		 */
		msg->sl.st.l = ptr - msg_start - msg->sol;
		return ptr;

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