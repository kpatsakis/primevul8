ins_ctrl_o(void)
{
    if (State & VREPLACE_FLAG)
	restart_edit = 'V';
    else
	if (State & REPLACE_FLAG)
	restart_edit = 'R';
    else
	restart_edit = 'I';
    if (virtual_active())
	ins_at_eol = FALSE;	// cursor always keeps its column
    else
	ins_at_eol = (gchar_cursor() == NUL);
}