ast_for_decorated(struct compiling *c, const node *n)
{
    /* decorated: decorators (classdef | funcdef) */
    stmt_ty thing = NULL;
    asdl_seq *decorator_seq = NULL;

    REQ(n, decorated);

    decorator_seq = ast_for_decorators(c, CHILD(n, 0));
    if (!decorator_seq)
      return NULL;

    assert(TYPE(CHILD(n, 1)) == funcdef ||
           TYPE(CHILD(n, 1)) == classdef);

    if (TYPE(CHILD(n, 1)) == funcdef) {
      thing = ast_for_funcdef(c, CHILD(n, 1), decorator_seq);
    } else if (TYPE(CHILD(n, 1)) == classdef) {
      thing = ast_for_classdef(c, CHILD(n, 1), decorator_seq);
    }
    /* we count the decorators in when talking about the class' or
       function's line number */
    if (thing) {
        thing->lineno = LINENO(n);
        thing->col_offset = n->n_col_offset;
    }
    return thing;
}