#define GHCHECKSUMS_APP
#include <ghchecksum.h>

#include <cstdio>

using namespace GHChecksum;

int main(int argc, char **argv) {
  GHChecksumLib inst;

  if (argc > 0) {
    String input;
    String debug;
    inst.LoadText(argv[1], &input);
    for (int i = 1; i < argc; ++i) {
      inst.GetROQValues(input, &debug);

      /*u32 qbKey = inst.GetQBKey(input);
      printf("%s\t0x%08x\n", input.Get(), qbKey);*/
    }
    inst.WriteText(debug, "debug.txt");
  }

  return 0;
}