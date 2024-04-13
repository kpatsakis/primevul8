process_anchor(struct parsed_tag *tag, char *tagbuf)
{
    if (parsedtag_need_reconstruct(tag)) {
	parsedtag_set_value(tag, ATTR_HSEQ, Sprintf("%d", cur_hseq++)->ptr);
	return parsedtag2str(tag);
    }
    else {
	Str tmp = Sprintf("<a hseq=\"%d\"", cur_hseq++);
	Strcat_charp(tmp, tagbuf + 2);
	return tmp;
    }
}