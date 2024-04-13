set_breakpoint(struct readbuffer *obuf, int tag_length)
{
    obuf->bp.len = obuf->line->length;
    obuf->bp.pos = obuf->pos;
    obuf->bp.tlen = tag_length;
    obuf->bp.flag = obuf->flag;
#ifdef FORMAT_NICE
    obuf->bp.flag &= ~RB_FILL;
#endif				/* FORMAT_NICE */
    obuf->bp.top_margin = obuf->top_margin;
    obuf->bp.bottom_margin = obuf->bottom_margin;

    if (!obuf->bp.init_flag)
	return;

    bcopy((void *)&obuf->anchor, (void *)&obuf->bp.anchor,
	  sizeof(obuf->anchor));
    obuf->bp.img_alt = obuf->img_alt;
    obuf->bp.input_alt = obuf->input_alt;
    obuf->bp.in_bold = obuf->in_bold;
    obuf->bp.in_italic = obuf->in_italic;
    obuf->bp.in_under = obuf->in_under;
    obuf->bp.in_strike = obuf->in_strike;
    obuf->bp.in_ins = obuf->in_ins;
    obuf->bp.nobr_level = obuf->nobr_level;
    obuf->bp.prev_ctype = obuf->prev_ctype;
    obuf->bp.init_flag = 0;
}