void ass_set_check_readorder(ASS_Track *track, int check_readorder)
{
    track->parser_priv->check_readorder = check_readorder == 1;
}