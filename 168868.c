xps_debug_path(xps_context_t *ctx)
{
    segment *seg;
    curve_segment *cseg;

    seg = (segment*)ctx->pgs->path->first_subpath;
    while (seg)
    {
        switch (seg->type)
        {
        case s_start:
            dmprintf2(ctx->memory, "%g %g moveto\n",
                      fixed2float(seg->pt.x) * 0.001,
                      fixed2float(seg->pt.y) * 0.001);
            break;
        case s_line:
            dmprintf2(ctx->memory, "%g %g lineto\n",
                      fixed2float(seg->pt.x) * 0.001,
                      fixed2float(seg->pt.y) * 0.001);
            break;
        case s_line_close:
            dmputs(ctx->memory, "closepath\n");
            break;
        case s_curve:
            cseg = (curve_segment*)seg;
            dmprintf6(ctx->memory, "%g %g %g %g %g %g curveto\n",
                      fixed2float(cseg->p1.x) * 0.001,
                      fixed2float(cseg->p1.y) * 0.001,
                      fixed2float(cseg->p2.x) * 0.001,
                      fixed2float(cseg->p2.y) * 0.001,
                      fixed2float(seg->pt.x) * 0.001,
                      fixed2float(seg->pt.y) * 0.001);
            break;
        }
        seg = seg->next;
    }
}