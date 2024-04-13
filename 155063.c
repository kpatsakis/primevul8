win_enter(win_T *wp, int undo_sync)
{
    win_enter_ext(wp, undo_sync, FALSE, FALSE, TRUE, TRUE);
}