htmlFindEncoding(xmlParserCtxtPtr ctxt) {
    const xmlChar *start, *cur, *end;

    if ((ctxt == NULL) || (ctxt->input == NULL) ||
        (ctxt->input->encoding != NULL) || (ctxt->input->buf == NULL) ||
        (ctxt->input->buf->encoder != NULL))
        return(NULL);
    if ((ctxt->input->cur == NULL) || (ctxt->input->end == NULL))
        return(NULL);

    start = ctxt->input->cur;
    end = ctxt->input->end;
    /* we also expect the input buffer to be zero terminated */
    if (*end != 0)
        return(NULL);

    cur = xmlStrcasestr(start, BAD_CAST "HTTP-EQUIV");
    if (cur == NULL)
        return(NULL);
    cur = xmlStrcasestr(cur, BAD_CAST  "CONTENT");
    if (cur == NULL)
        return(NULL);
    cur = xmlStrcasestr(cur, BAD_CAST  "CHARSET=");
    if (cur == NULL)
        return(NULL);
    cur += 8;
    start = cur;
    while (((*cur >= 'A') && (*cur <= 'Z')) ||
           ((*cur >= 'a') && (*cur <= 'z')) ||
           ((*cur >= '0') && (*cur <= '9')) ||
           (*cur == '-') || (*cur == '_') || (*cur == ':') || (*cur == '/'))
           cur++;
    if (cur == start)
        return(NULL);
    return(xmlStrndup(start, cur - start));
}