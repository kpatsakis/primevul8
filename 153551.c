is_dump_text_type(char *type)
{
    struct mailcap *mcap;
    return (type && (mcap = searchExtViewer(type)) &&
	    (mcap->flags & (MAILCAP_HTMLOUTPUT | MAILCAP_COPIOUSOUTPUT)));
}