resolve_iffeature_getsizes(struct lys_iffeature *iffeat, unsigned int *expr_size, unsigned int *feat_size)
{
    unsigned int e = 0, f = 0, r = 0;
    uint8_t op;

    assert(iffeat);

    if (!iffeat->expr) {
        goto result;
    }

    do {
        op = iff_getop(iffeat->expr, e++);
        switch (op) {
        case LYS_IFF_NOT:
            if (!r) {
                r += 1;
            }
            break;
        case LYS_IFF_AND:
        case LYS_IFF_OR:
            if (!r) {
                r += 2;
            } else {
                r += 1;
            }
            break;
        case LYS_IFF_F:
            f++;
            if (r) {
                r--;
            }
            break;
        }
    } while(r);

result:
    if (expr_size) {
        *expr_size = e;
    }
    if (feat_size) {
        *feat_size = f;
    }
}