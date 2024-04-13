ast_for_slice(struct compiling *c, const node *n)
{
    node *ch;
    expr_ty lower = NULL, upper = NULL, step = NULL;

    REQ(n, subscript);

    /*
       subscript: '.' '.' '.' | test | [test] ':' [test] [sliceop]
       sliceop: ':' [test]
    */
    ch = CHILD(n, 0);
    if (TYPE(ch) == DOT)
        return Ellipsis(c->c_arena);

    if (NCH(n) == 1 && TYPE(ch) == test) {
        /* 'step' variable hold no significance in terms of being used over
           other vars */
        step = ast_for_expr(c, ch);
        if (!step)
            return NULL;

        return Index(step, c->c_arena);
    }

    if (TYPE(ch) == test) {
        lower = ast_for_expr(c, ch);
        if (!lower)
            return NULL;
    }

    /* If there's an upper bound it's in the second or third position. */
    if (TYPE(ch) == COLON) {
        if (NCH(n) > 1) {
            node *n2 = CHILD(n, 1);

            if (TYPE(n2) == test) {
                upper = ast_for_expr(c, n2);
                if (!upper)
                    return NULL;
            }
        }
    } else if (NCH(n) > 2) {
        node *n2 = CHILD(n, 2);

        if (TYPE(n2) == test) {
            upper = ast_for_expr(c, n2);
            if (!upper)
                return NULL;
        }
    }

    ch = CHILD(n, NCH(n) - 1);
    if (TYPE(ch) == sliceop) {
        if (NCH(ch) == 1) {
            /*
              This is an extended slice (ie "x[::]") with no expression in the
              step field. We set this literally to "None" in order to
              disambiguate it from x[:]. (The interpreter might have to call
              __getslice__ for x[:], but it must call __getitem__ for x[::].)
            */
            identifier none = new_identifier("None", c->c_arena);
            if (!none)
                return NULL;
            ch = CHILD(ch, 0);
            step = Name(none, Load, LINENO(ch), ch->n_col_offset, c->c_arena);
            if (!step)
                return NULL;
        } else {
            ch = CHILD(ch, 1);
            if (TYPE(ch) == test) {
                step = ast_for_expr(c, ch);
                if (!step)
                    return NULL;
            }
        }
    }

    return Slice(lower, upper, step, c->c_arena);
}