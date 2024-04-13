back_to_breakpoint(struct readbuffer *obuf)
{
    obuf->flag = obuf->bp.flag;
    bcopy((void *)&obuf->bp.anchor, (void *)&obuf->anchor,
	  sizeof(obuf->anchor));
    obuf->img_alt = obuf->bp.img_alt;
    obuf->input_alt = obuf->bp.input_alt;
    obuf->in_bold = obuf->bp.in_bold;
    obuf->in_italic = obuf->bp.in_italic;
    obuf->in_under = obuf->bp.in_under;
    obuf->in_strike = obuf->bp.in_strike;
    obuf->in_ins = obuf->bp.in_ins;
    obuf->prev_ctype = obuf->bp.prev_ctype;
    obuf->pos = obuf->bp.pos;
    obuf->top_margin = obuf->bp.top_margin;
    obuf->bottom_margin = obuf->bp.bottom_margin;
    if (obuf->flag & RB_NOBR)
	obuf->nobr_level = obuf->bp.nobr_level;
}