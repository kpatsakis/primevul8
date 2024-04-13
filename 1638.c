Node::NodeClass Node::GetNodeClassForOp(const std::string& ts) {
  static const absl::flat_hash_map<std::string, Node::NodeClass>*
      kNodeClassTable =
#define REF_CLASS(key, value) \
  {key, value}, { "Ref" key, value }
          new absl::flat_hash_map<std::string, Node::NodeClass>({
              // Keep in same order as NodeClass values
              REF_CLASS("Switch", NC_SWITCH),
              REF_CLASS("_SwitchN", NC_SWITCH),
              REF_CLASS("Merge", NC_MERGE),
              REF_CLASS("Enter", NC_ENTER),
              REF_CLASS("Exit", NC_EXIT),
              REF_CLASS("NextIteration", NC_NEXT_ITERATION),
              {"LoopCond", NC_LOOP_COND},
              {"ControlTrigger", NC_CONTROL_TRIGGER},
              {"_Send", NC_SEND},
              {"_HostSend", NC_HOST_SEND},
              {"_Recv", NC_RECV},
              {"_HostRecv", NC_HOST_RECV},
              {"Const", NC_CONSTANT},
              {"HostConst", NC_CONSTANT},
              {"Variable", NC_VARIABLE},
              {"VariableV2", NC_VARIABLE},
              REF_CLASS("Identity", NC_IDENTITY),
              {"GetSessionHandle", NC_GET_SESSION_HANDLE},
              {"GetSessionHandleV2", NC_GET_SESSION_HANDLE},
              {"GetSessionTensor", NC_GET_SESSION_TENSOR},
              {"DeleteSessionTensor", NC_DELETE_SESSION_TENSOR},
              {"Size", NC_METADATA},
              {"Shape", NC_METADATA},
              {"Rank", NC_METADATA},
              {"_ScopedAllocator", NC_SCOPED_ALLOCATOR},
              {"CollectiveReduce", NC_COLLECTIVE},
              {"CollectiveBcastSend", NC_COLLECTIVE},
              {"CollectiveBcastRecv", NC_COLLECTIVE},
              {"CollectiveGather", NC_COLLECTIVE},
              {"FakeParam", NC_FAKE_PARAM},
              {"PartitionedCall", NC_PARTITIONED_CALL},
              {"StatefulPartitionedCall", NC_PARTITIONED_CALL},
              {"SymbolicGradient", NC_SYMBOLIC_GRADIENT},
              {"If", NC_IF},
              {"StatelessIf", NC_IF},
              {"While", NC_WHILE},
              {"StatelessWhile", NC_WHILE},
              {"Case", NC_CASE},
              {"StatelessCase", NC_CASE},
              // Not using the constants defined in FunctionLibraryDefinition
              // for the
              // 4 ops below because android inference library does not link
              // tf.function related files.
              {"_Arg", NC_ARG},
              {"_DeviceArg", NC_ARG},
              {"_Retval", NC_RETVAL},
              {"_DeviceRetval", NC_RETVAL},
              {"_XlaMerge", NC_MERGE},
          });
#undef REF_CLASS

  auto it = kNodeClassTable->find(ts);
  if (it != kNodeClassTable->end()) {
    return it->second;
  } else {
    return NC_OTHER;
  }
}