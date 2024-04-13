void Statement::Process() {
    if (finalized && !queue.empty()) {
        return CleanQueue();
    }

    while (prepared && !locked && !queue.empty()) {
        std::unique_ptr<Call> call(queue.front());
        queue.pop();

        call->callback(call->baton);
    }
}