catch(std::exception& e)
{
  cerr<<"Receiver function died: "<<e.what()<<endl;
  exit(1);
}