process_idattr(struct readbuffer *obuf, int cmd, struct parsed_tag *tag)
{
    char *id = NULL, *framename = NULL;
    Str idtag = NULL;

    /* 
     * HTML_TABLE is handled by the other process.
     */
    if (cmd == HTML_TABLE)
	return;

    parsedtag_get_value(tag, ATTR_ID, &id);
    parsedtag_get_value(tag, ATTR_FRAMENAME, &framename);
    if (id == NULL)
	return;
    if (framename)
	idtag = Sprintf("<_id id=\"%s\" framename=\"%s\">",
			html_quote(id), html_quote(framename));
    else
	idtag = Sprintf("<_id id=\"%s\">", html_quote(id));
    push_tag(obuf, idtag->ptr, HTML_NOP);
}