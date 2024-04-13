int get_user_page_nowait(struct task_struct *tsk, struct mm_struct *mm,
	unsigned long start, int write, struct page **page)
{
	int flags = FOLL_TOUCH | FOLL_NOWAIT | FOLL_HWPOISON | FOLL_GET;

	if (write)
		flags |= FOLL_WRITE;

	return __get_user_pages(tsk, mm, start, 1, flags, page, NULL, NULL);
}