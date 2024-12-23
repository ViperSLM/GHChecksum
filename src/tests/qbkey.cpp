#define GHCHECKSUMS_APP
#include <ghchecksum.h>

#include <cstdio>

using namespace GHChecksum;

int main(int argc, char **argv) {
  GHChecksumLib inst(argc,argv);

  if (argc > 0) {
    String input;
    for (int i = 1; i < argc; ++i) {
      input.Set("%s", argv[i]);
      u32 qbKey = inst.GetQBKey(input);
      printf("%s\t%s\n", input.Get(), inst.QBKeyToString(qbKey));
    }
  }

  return 0;
}