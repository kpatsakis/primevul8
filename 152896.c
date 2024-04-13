Ta3Token_TwoChars(int c1, int c2)
{
    switch (c1) {
    case '=':
        switch (c2) {
        case '=':               return EQEQUAL;
        }
        break;
    case '!':
        switch (c2) {
        case '=':               return NOTEQUAL;
        }
        break;
    case '<':
        switch (c2) {
        case '>':               return NOTEQUAL;
        case '=':               return LESSEQUAL;
        case '<':               return LEFTSHIFT;
        }
        break;
    case '>':
        switch (c2) {
        case '=':               return GREATEREQUAL;
        case '>':               return RIGHTSHIFT;
        }
        break;
    case '+':
        switch (c2) {
        case '=':               return PLUSEQUAL;
        }
        break;
    case '-':
        switch (c2) {
        case '=':               return MINEQUAL;
        case '>':               return RARROW;
        }
        break;
    case '*':
        switch (c2) {
        case '*':               return DOUBLESTAR;
        case '=':               return STAREQUAL;
        }
        break;
    case '/':
        switch (c2) {
        case '/':               return DOUBLESLASH;
        case '=':               return SLASHEQUAL;
        }
        break;
    case '|':
        switch (c2) {
        case '=':               return VBAREQUAL;
        }
        break;
    case '%':
        switch (c2) {
        case '=':               return PERCENTEQUAL;
        }
        break;
    case '&':
        switch (c2) {
        case '=':               return AMPEREQUAL;
        }
        break;
    case '^':
        switch (c2) {
        case '=':               return CIRCUMFLEXEQUAL;
        }
        break;
    case '@':
        switch (c2) {
        case '=':               return ATEQUAL;
        }
        break;
    }
    return OP;
}