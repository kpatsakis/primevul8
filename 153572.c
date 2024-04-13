fillline(struct readbuffer *obuf, int indent)
{
    push_spaces(obuf, 1, indent - obuf->pos);
    obuf->flag &= ~RB_NFLUSHED;
}