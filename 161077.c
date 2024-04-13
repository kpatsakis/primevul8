replace_pop_ins(void)
{
    int	    cc;
    int	    oldState = State;

    State = NORMAL;			// don't want REPLACE here
    while ((cc = replace_pop()) > 0)
    {
	mb_replace_pop_ins(cc);
	dec_cursor();
    }
    State = oldState;
}