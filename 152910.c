set_context(struct compiling *c, expr_ty e, expr_context_ty ctx, const node *n)
{
    asdl_seq *s = NULL;
    /* If a particular expression type can't be used for assign / delete,
       set expr_name to its name and an error message will be generated.
    */
    const char* expr_name = NULL;

    /* The ast defines augmented store and load contexts, but the
       implementation here doesn't actually use them.  The code may be
       a little more complex than necessary as a result.  It also means
       that expressions in an augmented assignment have a Store context.
       Consider restructuring so that augmented assignment uses
       set_context(), too.
    */
    assert(ctx != AugStore && ctx != AugLoad);

    switch (e->kind) {
        case Attribute_kind:
            if (ctx == Store && !forbidden_check(c, n,
                                PyUnicode_AsUTF8(e->v.Attribute.attr)))
                    return 0;
            e->v.Attribute.ctx = ctx;
            break;
        case Subscript_kind:
            e->v.Subscript.ctx = ctx;
            break;
        case Name_kind:
            if (ctx == Store && !forbidden_check(c, n,
                                PyUnicode_AsUTF8(e->v.Name.id)))
                    return 0;
            e->v.Name.ctx = ctx;
            break;
        case List_kind:
            e->v.List.ctx = ctx;
            s = e->v.List.elts;
            break;
        case Tuple_kind:
            if (asdl_seq_LEN(e->v.Tuple.elts))  {
                e->v.Tuple.ctx = ctx;
                s = e->v.Tuple.elts;
            }
            else {
                expr_name = "()";
            }
            break;
        case Lambda_kind:
            expr_name = "lambda";
            break;
        case Call_kind:
            expr_name = "function call";
            break;
        case BoolOp_kind:
        case BinOp_kind:
        case UnaryOp_kind:
            expr_name = "operator";
            break;
        case GeneratorExp_kind:
            expr_name = "generator expression";
            break;
        case Yield_kind:
            expr_name = "yield expression";
            break;
        case ListComp_kind:
            expr_name = "list comprehension";
            break;
        case SetComp_kind:
            expr_name = "set comprehension";
            break;
        case DictComp_kind:
            expr_name = "dict comprehension";
            break;
        case Dict_kind:
        case Set_kind:
        case Num_kind:
        case Str_kind:
            expr_name = "literal";
            break;
        case Compare_kind:
            expr_name = "comparison";
            break;
        case Repr_kind:
            expr_name = "repr";
            break;
        case IfExp_kind:
            expr_name = "conditional expression";
            break;
        default:
            PyErr_Format(PyExc_SystemError,
                         "unexpected expression in assignment %d (line %d)",
                         e->kind, e->lineno);
            return 0;
    }
    /* Check for error string set by switch */
    if (expr_name) {
        char buf[300];
        PyOS_snprintf(buf, sizeof(buf),
                      "can't %s %s",
                      ctx == Store ? "assign to" : "delete",
                      expr_name);
        return ast_error(n, buf);
    }

    /* If the LHS is a list or tuple, we need to set the assignment
       context for all the contained elements.
    */
    if (s) {
        int i;

        for (i = 0; i < asdl_seq_LEN(s); i++) {
            if (!set_context(c, (expr_ty)asdl_seq_GET(s, i), ctx, n))
                return 0;
        }
    }
    return 1;
}