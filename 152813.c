Ta3Token_OneChar(int c)
{
    switch (c) {
    case '(':           return LPAR;
    case ')':           return RPAR;
    case '[':           return LSQB;
    case ']':           return RSQB;
    case ':':           return COLON;
    case ',':           return COMMA;
    case ';':           return SEMI;
    case '+':           return PLUS;
    case '-':           return MINUS;
    case '*':           return STAR;
    case '/':           return SLASH;
    case '|':           return VBAR;
    case '&':           return AMPER;
    case '<':           return LESS;
    case '>':           return GREATER;
    case '=':           return EQUAL;
    case '.':           return DOT;
    case '%':           return PERCENT;
    case '{':           return LBRACE;
    case '}':           return RBRACE;
    case '^':           return CIRCUMFLEX;
    case '~':           return TILDE;
    case '@':           return AT;
    default:            return OP;
    }
}