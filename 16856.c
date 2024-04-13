ins_tabline(int c)
{
    // We will be leaving the current window, unless closing another tab.
    if (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_index(curtab)))
    {
	undisplay_dollar();
	start_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    if (c == K_TABLINE)
	goto_tabpage(current_tab);
    else
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}