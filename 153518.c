push_link(int cmd, int offset, int pos)
{
    struct link_stack *p;
    p = New(struct link_stack);
    p->cmd = cmd;
    p->offset = offset;
    p->pos = pos;
    p->next = link_stack;
    link_stack = p;
}