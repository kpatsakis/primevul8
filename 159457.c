set_cursor_for_append_to_line(void)
{
    curwin->w_set_curswant = TRUE;
    if (get_ve_flags() == VE_ALL)
    {
	int save_State = State;

	// Pretend Insert mode here to allow the cursor on the
	// character past the end of the line
	State = INSERT;
	coladvance((colnr_T)MAXCOL);
	State = save_State;
    }
    else
	curwin->w_cursor.col += (colnr_T)STRLEN(ml_get_cursor());
}