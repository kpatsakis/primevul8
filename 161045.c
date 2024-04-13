init_prompt(int cmdchar_todo)
{
    char_u *prompt = prompt_text();
    char_u *text;

    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
    text = ml_get_curline();
    if (STRNCMP(text, prompt, STRLEN(prompt)) != 0)
    {
	// prompt is missing, insert it or append a line with it
	if (*text == NUL)
	    ml_replace(curbuf->b_ml.ml_line_count, prompt, TRUE);
	else
	    ml_append(curbuf->b_ml.ml_line_count, prompt, 0, FALSE);
	curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	coladvance((colnr_T)MAXCOL);
	changed_bytes(curbuf->b_ml.ml_line_count, 0);
    }

    // Insert always starts after the prompt, allow editing text after it.
    if (Insstart_orig.lnum != curwin->w_cursor.lnum
				   || Insstart_orig.col != (int)STRLEN(prompt))
    {
	Insstart.lnum = curwin->w_cursor.lnum;
	Insstart.col = (int)STRLEN(prompt);
	Insstart_orig = Insstart;
	Insstart_textlen = Insstart.col;
	Insstart_blank_vcol = MAXCOL;
	arrow_used = FALSE;
    }

    if (cmdchar_todo == 'A')
	coladvance((colnr_T)MAXCOL);
    if (cmdchar_todo == 'I' || curwin->w_cursor.col <= (int)STRLEN(prompt))
	curwin->w_cursor.col = (int)STRLEN(prompt);
    // Make sure the cursor is in a valid position.
    check_cursor();
}