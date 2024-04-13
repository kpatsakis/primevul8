set_alignment(struct readbuffer *obuf, struct parsed_tag *tag)
{
    long flag = -1;
    int align;

    if (parsedtag_get_value(tag, ATTR_ALIGN, &align)) {
	switch (align) {
	case ALIGN_CENTER:
	    flag = RB_CENTER;
	    break;
	case ALIGN_RIGHT:
	    flag = RB_RIGHT;
	    break;
	case ALIGN_LEFT:
	    flag = RB_LEFT;
	}
    }
    RB_SAVE_FLAG(obuf);
    if (flag != -1) {
	RB_SET_ALIGN(obuf, flag);
    }
}