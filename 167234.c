static void outputfiles(int f, void * const tls_fd)
{
    unsigned int n;
    struct filename *p;
    struct filename *q;

    if (!head) {
        return;
    }
    tail->down = NULL;
    tail = NULL;
    colwidth = (colwidth | 7U) + 1U;
    if (opt_l != 0 || opt_C == 0) {
        colwidth = 75U;
    }
    /* set up first column */
    p = head;
    p->top = 1;
    if (colwidth > 75U) {
        n = filenames;
    } else {
        n = (filenames + (75U / colwidth) - 1U) / (75U / colwidth);
    }
    while (n && p) {
        p = p->down;
        if (p != NULL) {
            p->top = 0;
        }
        n--;
    }

    /* while there's a neighbour to the right, point at it */
    q = head;
    while (p) {
        p->top = q->top;
        q->right = p;
        q = q->down;
        p = p->down;
    }

    /* some are at the right end */
    while (q) {
        q->right = NULL;
        q = q->down;
    }

    /* don't want wraparound, do we? */
    p = head;
    while (p && p->down && !p->down->top) {
        p = p->down;
    }
    if (p && p->down) {
        p->down = NULL;
    }

    /* print each line, which consists of each column */
    p = head;
    while (p) {
        q = p;
        p = p->down;
        while (q) {
            char pad[6];
            char *tmp = (char *) q;

            if (q->right) {
                memset(pad, '\t', sizeof pad - 1U);
                pad[(sizeof pad) - 1] = 0;
                pad[(colwidth + 7U - strlen(q->line)) / 8] = 0;
            } else {
                pad[0] = '\r';
                pad[1] = '\n';
                pad[2] = 0;
            }
            wrstr(f, tls_fd, q->line);
            wrstr(f, tls_fd, pad);
            q = q->right;
            free(tmp);
            tmp = NULL;
        }
    }

    /* reset variables for next time */
    head = tail = NULL;
    colwidth = 0U;
    filenames = 0U;
}