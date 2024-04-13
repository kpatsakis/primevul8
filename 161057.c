prompt_curpos_editable()
{
    return curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count
	&& curwin->w_cursor.col >= (int)STRLEN(prompt_text());
}