#ifndef GH_CONVCHECKSUM_H
#define GH_CONVCHECKSUM_H
#include "GHChecksum_API.h"
#include "gh_inttypes.h"

#include "CustomString.h"

namespace GHChecksum {
class APP_API GHChecksumLib {
public:

  /* Constructor: Parses command line arguments */
  GHChecksumLib(int argc = 0, char **argv = nullptr);

  /* Destructor: Does nothing, but does invoke the _impl's
  default delete function. */
  ~GHChecksumLib(void);

  /* Usage message */
  void UsageMsg(void);

  /* Parse checksums table to unordered_map object */
  void ParseChecksumsFile(String &input);

  /* Write text to file */
  void WriteText(String &input, const char *fileName);

  /* Process the QBC script with the loaded checksums table
     and output to *output pointer */
  void ProcessQBC(String &input, String *output);

  /* Load text from file and store into *outputStr pointer */
  bool LoadText(const char *filePath, String *outputStr);

  /* Does the input string contain a specific keyword? */
  bool Contains(String &input, const char *txt);
  bool Contains(const char *input, const char *txt);
  bool HasArg(const char *arg, usize *foundArgIndex = nullptr);

  /* Look for unresolved checksums in QBC script
   and return a string containing a list of all checksums found. */
  void GetChecksums(String &input, String *output = nullptr);

  // Generate CRC32 hash bashed on input
  u32 GetQBKey(const char *input);

  void GetROQValues(String &input);

  bool IsROQScript(String &input);

  void ProcessChecksums(void);

  void OutputChecksums(String *output);
  void OutputChecksums_QB(String *output);

private:
  /* Pointer to implementation [PIMPL] */
  class GHChecksumLib_Impl;
#if defined(_WIN32) && !defined(GH_STATIC_LIB)
  GHChecksumLib_Impl *_impl = nullptr;
#else
  LIB_PIMPL<GHChecksumLib_Impl> _impl;
#endif
};
}

#endif