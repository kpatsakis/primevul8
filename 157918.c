ClassDef(identifier name, asdl_seq * bases, asdl_seq * keywords, asdl_seq *
         body, asdl_seq * decorator_list, int lineno, int col_offset, int
         end_lineno, int end_col_offset, PyArena *arena)
{
    stmt_ty p;
    if (!name) {
        PyErr_SetString(PyExc_ValueError,
                        "field name is required for ClassDef");
        return NULL;
    }
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = ClassDef_kind;
    p->v.ClassDef.name = name;
    p->v.ClassDef.bases = bases;
    p->v.ClassDef.keywords = keywords;
    p->v.ClassDef.body = body;
    p->v.ClassDef.decorator_list = decorator_list;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}