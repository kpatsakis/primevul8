burn_stack (int bytes)
{
    char buf[128];

    wipememory(buf,sizeof buf);
    bytes -= sizeof buf;
    if (bytes > 0)
        burn_stack (bytes);
}