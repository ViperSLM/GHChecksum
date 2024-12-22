#include "ghchecksum.h"

#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <vector>
#include <regex>

/* Pattern used in the library to help find QBKeys [ Format is #"0x00000000" - #"0xffffffff" ] */
constexpr const char *QB_PATTERN = R"(\#\"0x[0-9A-Fa-f]+\")";

namespace GHChecksum {

/* -------------------------------------------------- */
/* GHChecksumLib - Private Implementation */
/* -------------------------------------------------- */

class GHChecksumLib::GHChecksumLib_Impl {
public:
  GHChecksumLib_Impl(int argc, char **argv) {
    _convertedArgs.clear();
    for (int i = 0; i < argc; ++i) {
      _convertedArgs.push_back(argv[i]);
    }
  }
  ~GHChecksumLib_Impl(void) {}

  void usageMsg(void) {
      printf("Usage:\nGHChecksum.exe [Input .q file] [Output .checksums file]\nOR\nGHChecksum.exe [Input .checksums file] [Input .q file]\n");
  }

  bool hasArg(const char *arg, size_t *foundArgIndex) {
    std::string arg_toCompare = arg;
    for (size_t i = 1; i < _convertedArgs.size(); ++i) {
      if (arg_toCompare == _convertedArgs[i]) {
        if (foundArgIndex != nullptr) {
          *foundArgIndex = i;
        }
        return true;
      }
    }
    return false;
  }

  bool loadTextFile(const char *filePath, String *outputStr) {
    FILE *filePtr = fopen(filePath, "r");

    if (filePtr != nullptr) {
      char chBuffer;
      outputStr->Clear();

      while ((chBuffer = fgetc(filePtr)) != EOF) {
        outputStr->Append("%c", chBuffer);
      }
      fclose(filePtr);
      return true;
    }
    // Failed
    return false;
  }

  void writeTextFile(String &input, const char *fileName) {
    FILE *filePtr = fopen(fileName, "w");
    fprintf(filePtr, "%s", input.Get());
    fclose(filePtr);
  }

  void getChecksums(String &input, String *output) {
    std::string strInput = input.Get();
    std::string strOutput;

    std::regex pattern(QB_PATTERN);
    auto matches_begin =
        std::sregex_iterator(strInput.begin(), strInput.end(), pattern);

    auto matches_end = std::sregex_iterator();

    std::vector<std::string> uniqueMatches;
    std::unordered_set<std::string> seen;

    for (std::sregex_iterator it = matches_begin; it != matches_end; ++it) {
      std::string match = it->str();
      if (seen.insert(match).second) {
        uniqueMatches.push_back(match);
      }
    }

    int counter = 0;
    // Output all unique matches to output string
    for (const auto &match : uniqueMatches) {
      strOutput += match + '\n';
      counter++;
    }
    printf("%d QBKeys found in script\n", counter);

    // Return strOutput to the output pointer
    output->Clear();
    output->Set("%s", strOutput.c_str());
  }

  bool contains(String &input, const char *txt) {
    std::string strInput = input.Get();
    return strInput.find(txt) != std::string::npos;
  }

  void parseChecksumsFile(String &input) {
    std::string strInput = input.Get();
    std::istringstream stream(strInput);
    std::string line;

    while (std::getline(stream, line)) {
      // Split line into QBKey and value
      size_t separator = line.find("  "); // Two spaces
      if (separator != std::string::npos) {
        std::string qbKey = line.substr(0, separator);
        std::string origValue = line.substr(separator + 2);
        _checksumList[qbKey] = origValue;
      }
    }
  }

  void replaceQBKeys(String &input, String *output) {
    output->Clear();
    int counter = 0;
    std::string qbc = input.Get();

    for (const auto &pair : _checksumList) {
      const std::string &QBKey = pair.first;
      const std::string &OrigValue = pair.second;

      size_t pos = 0;
      while ((pos = qbc.find(QBKey, pos)) != std::string::npos) {
        qbc.replace(pos, QBKey.length(), OrigValue);
        pos += OrigValue.length();

        counter++;
      }
    }
    printf("Processed %d QBKeys", counter);
    output->Set("%s", qbc.c_str()); // Set contents of output to the processed QBC
  }

private:
  std::vector<std::string> _convertedArgs;
  std::unordered_map<std::string, std::string> _checksumList;
};

/* -------------------------------------------------- */
/* GHChecksumLib - Public Implementation */
/* -------------------------------------------------- */

#if defined(_WIN32) && !defined(GH_STATIC_LIB)
GHChecksumLib::GHChecksumLib(int argc, char **argv) : _impl(new GHChecksumLib_Impl(argc,argv)) {}
GHChecksumLib::~GHChecksumLib(void) {
  delete _impl;
  _impl = nullptr;
}
#else
GHChecksumLib::GHChecksumLib(int argc, char **argv)
    : _impl(LIB_PIMPL_CREATE<GHChecksumLib_Impl>(argc, argv)) {}
GHChecksumLib::~GHChecksumLib(void) = default;
#endif

void GHChecksumLib::UsageMsg(void) { _impl->usageMsg(); }

void GHChecksumLib::ParseChecksumsFile(String &input) {
  _impl->parseChecksumsFile(input);
}

void GHChecksumLib::WriteText(String &input,
                                                  const char *fileName) {
  _impl->writeTextFile(input, fileName);
}

void GHChecksumLib::ProcessQBC(String &input, String *output) {
  _impl->replaceQBKeys(input, output);
}

bool GHChecksumLib::LoadText(const char *filePath,
                                                 String *outputStr) {
  return _impl->loadTextFile(filePath, outputStr);
}

bool GHChecksumLib::Contains(String &input,
                                                 const char *txt) {
  return _impl->contains(input, txt);
}

void GHChecksumLib::GetChecksums(String &input, String *output) {
  _impl->getChecksums(input, output);
}

}