#ifndef GH_CONVCHECKSUM_H
#define GH_CONVCHECKSUM_H

#include <string>
#include <memory>

namespace GHChecksum {
class ChecksumConversionApp {
public:

  /* Constructor: Parses command line arguments */
  ChecksumConversionApp(int argc, char **argv);

  /* Destructor: Does nothing, but does invoke the _impl's
  default delete function. */
  ~ChecksumConversionApp(void);

  /* Usage message */
  void UsageMsg(void);

  /* Parse checksums table to unordered_map object */
  void ParseChecksumsFile(std::string &input);

  /* Write text to file */
  void WriteText(std::string &input, const char *fileName);

  /* Process the QBC script with the loaded checksums table
     and output to *output pointer */
  void ProcessQBC(std::string &input, std::string *output);

  /* Load text from file and store into *outputStr pointer */
  bool LoadText(const char *filePath, std::string *outputStr);

  /* Does the input string contain a specific keyword? */
  bool Contains(std::string &input, const char *txt);

  /* Look for unresolved checksums in QBC script
   and return a string containing a list of all checksums found. */
  std::string GetChecksums(std::string &input);

private:
  /* Pointer to implementation [PIMPL] */
  class CCA_Impl;
  std::unique_ptr<CCA_Impl> _impl;
};
}

#endif