#define GHCHECKSUMS_APP
#include <ghchecksum.h>

#include <cstdio>

using namespace GHChecksum;

int main(int argc, char **argv) {
  GHChecksumLib inst;

  if (argc > 0) {
    String input;
    for (int i = 1; i < argc; ++i) {
      input.Set("%s", argv[i]);
      u32 qbKey = inst.GetQBKey(input.Get());
      printf("%s\t0x%08x\n", input.Get(), qbKey);
    }
  }

  return 0;
}