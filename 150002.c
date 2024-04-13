match_inspect(VALUE match)
{
    VALUE cname = rb_class_path(rb_obj_class(match));
    VALUE str;
    int i;
    struct re_registers *regs = RMATCH_REGS(match);
    int num_regs = regs->num_regs;
    struct backref_name_tag *names;
    VALUE regexp = RMATCH(match)->regexp;

    if (regexp == 0) {
        return rb_sprintf("#<%"PRIsVALUE":%p>", cname, (void*)match);
    }
    else if (NIL_P(regexp)) {
        return rb_sprintf("#<%"PRIsVALUE": %"PRIsVALUE">",
			  cname, rb_reg_nth_match(0, match));
    }

    names = ALLOCA_N(struct backref_name_tag, num_regs);
    MEMZERO(names, struct backref_name_tag, num_regs);

    onig_foreach_name(RREGEXP_PTR(regexp),
            match_inspect_name_iter, names);

    str = rb_str_buf_new2("#<");
    rb_str_append(str, cname);

    for (i = 0; i < num_regs; i++) {
        VALUE v;
        rb_str_buf_cat2(str, " ");
        if (0 < i) {
            if (names[i].name)
                rb_str_buf_cat(str, (const char *)names[i].name, names[i].len);
            else {
                rb_str_catf(str, "%d", i);
            }
            rb_str_buf_cat2(str, ":");
        }
        v = rb_reg_nth_match(i, match);
        if (v == Qnil)
            rb_str_buf_cat2(str, "nil");
        else
            rb_str_buf_append(str, rb_str_inspect(v));
    }
    rb_str_buf_cat2(str, ">");

    return str;
}