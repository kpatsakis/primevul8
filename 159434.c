prep_redo(
    int	    regname,
    long    num,
    int	    cmd1,
    int	    cmd2,
    int	    cmd3,
    int	    cmd4,
    int	    cmd5)
{
    ResetRedobuff();
    if (regname != 0)	// yank from specified buffer
    {
	AppendCharToRedobuff('"');
	AppendCharToRedobuff(regname);
    }
    if (num)
	AppendNumberToRedobuff(num);

    if (cmd1 != NUL)
	AppendCharToRedobuff(cmd1);
    if (cmd2 != NUL)
	AppendCharToRedobuff(cmd2);
    if (cmd3 != NUL)
	AppendCharToRedobuff(cmd3);
    if (cmd4 != NUL)
	AppendCharToRedobuff(cmd4);
    if (cmd5 != NUL)
	AppendCharToRedobuff(cmd5);
}