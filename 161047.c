prompt_text(void)
{
    if (curbuf->b_prompt_text == NULL)
	return (char_u *)"% ";
    return curbuf->b_prompt_text;
}