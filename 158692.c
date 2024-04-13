mainlog_close(void)
{
if (mainlogfd < 0) return;
(void)close(mainlogfd);
mainlogfd = -1;
mainlog_inode = 0;
}