static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void *scanner, struct yang_parameter *param)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (scanner);
  YYUSE (param);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
          case 115: /* tmp_string  */

      { free((((*yyvaluep).p_str)) ? *((*yyvaluep).p_str) : NULL); }

        break;

    case 210: /* pattern_arg_str  */

      { free(((*yyvaluep).str)); }

        break;

    case 399: /* semicolom  */

      { free(((*yyvaluep).str)); }

        break;

    case 401: /* curly_bracket_open  */

      { free(((*yyvaluep).str)); }

        break;

    case 405: /* string_opt_part1  */

      { free(((*yyvaluep).str)); }

        break;

    case 430: /* type_ext_alloc  */

      { yang_type_free(param->module->ctx, ((*yyvaluep).v)); }

        break;

    case 431: /* typedef_ext_alloc  */

      { yang_type_free(param->module->ctx, &((struct lys_tpdf *)((*yyvaluep).v))->type); }

        break;


      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END