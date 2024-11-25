{
  const std::string user = gSystem->Getenv("USER");
  if(user == "leo_leonis"){
    gSystem->AddIncludePath("-I/home/leo_leonis/pythia8312//include/");
    gSystem->Load("/home/leo_leonis/pythia8312//lib/libpythia8");
  }
}