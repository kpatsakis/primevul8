void _RemoveElement(cmsStage** head)
{
    cmsStage* mpe = *head;
    cmsStage* next = mpe ->Next;
    *head = next;
    cmsStageFree(mpe);
}