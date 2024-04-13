close_effect0(struct readbuffer *obuf, int cmd)
{
    int i;
    char *p;

    for (i = obuf->tag_sp - 1; i >= 0; i--) {
	if (obuf->tag_stack[i]->cmd == cmd)
	    break;
    }
    if (i >= 0) {
	obuf->tag_sp--;
	bcopy(&obuf->tag_stack[i + 1], &obuf->tag_stack[i],
	      (obuf->tag_sp - i) * sizeof(struct cmdtable *));
	return 1;
    }
    else if ((p = has_hidden_link(obuf, cmd)) != NULL) {
	passthrough(obuf, p, 1);
	return 1;
    }
    return 0;
}