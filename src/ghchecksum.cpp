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

constexpr const char *QB_PATTERN = R"(\#\"0x[0-9A-Fa-f]+\")";

namespace GHChecksum {
class ChecksumConversionApp::CCA_Impl {
public:
  CCA_Impl(int argc, char **argv) {
    _convertedArgs.clear();
    for (int i = 0; i < argc; ++i) {
      _convertedArgs.push_back(argv[i]);
    }
  }
  ~CCA_Impl(void) {}

  void usageMsg(void) {
      printf("Usage:\nGHChecksum.exe [Input .q file] [Output .checksums file]\n\nOR\n\nGHChecksum.exe [Input .checksums file] [Input .q file]");
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

  bool loadTextFile(const char *filePath, std::string *outputStr) {
    FILE *filePtr = fopen(filePath, "r");

    if (filePtr != nullptr) {
      char chBuffer;
      outputStr->clear();

      while ((chBuffer = fgetc(filePtr)) != EOF) {
        *outputStr += chBuffer;
      }
      fclose(filePtr);
      return true;
    }
    // Failed
    return false;
  }

  void writeTextFile(std::string &input, const char *fileName) {
    FILE *filePtr = fopen(fileName, "w");
    fprintf(filePtr, "%s", input.c_str());
    fclose(filePtr);
  }

  std::string getChecksums(std::string &input) {
    std::string output;
    std::regex pattern(QB_PATTERN);
    auto matches_begin =
        std::sregex_iterator(input.begin(), input.end(), pattern);

    auto matches_end = std::sregex_iterator();

    std::vector<std::string> uniqueMatches;
    std::unordered_set<std::string> seen;

    for (std::sregex_iterator it = matches_begin; it != matches_end; ++it) {
      std::string match = it->str();
      if (seen.insert(match).second) {
        uniqueMatches.push_back(match);
      }
    }

    // Output all unique matches to output string
    for (const auto &match : uniqueMatches) {
      output += match + '\n';
    }

    return output;
  }

  bool contains(std::string &input, std::string txt) {
    return input.find(txt) != std::string::npos;
  }

  void parseChecksumsFile(std::string &input) {
    std::istringstream stream(input);
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

  void replaceQBKeys(std::string& input, std::string* output) {
    output->clear();
    std::string qbc = input;

    for (const auto &pair : _checksumList) {
      const std::string &QBKey = pair.first;
      const std::string &OrigValue = pair.second;

      size_t pos = 0;
      while ((pos = qbc.find(QBKey, pos)) != std::string::npos) {
        qbc.replace(pos, QBKey.length(), OrigValue);
        pos += OrigValue.length();
      }
    }
    *output = qbc; // Set contents of output to the processed QBC
  }

private:
  std::vector<std::string> _convertedArgs;
  std::unordered_map<std::string, std::string> _checksumList;
};

ChecksumConversionApp::ChecksumConversionApp(int argc, char **argv)
    : _impl(std::make_unique<CCA_Impl>(argc, argv)) {}
ChecksumConversionApp::~ChecksumConversionApp(void) = default;

void ChecksumConversionApp::UsageMsg(void) { _impl->usageMsg(); }

bool ChecksumConversionApp::LoadText(const char *filePath,
                                     std::string *outputStr) {
  return _impl->loadTextFile(filePath, outputStr);
}

std::string ChecksumConversionApp::GetChecksums(std::string &input) {
  return _impl->getChecksums(input);
}

void ChecksumConversionApp::WriteText(std::string &input,
                                      const char *fileName) {
  _impl->writeTextFile(input, fileName);
}

void ChecksumConversionApp::ParseChecksumsFile(std::string &input) {
  _impl->parseChecksumsFile(input);
}

bool ChecksumConversionApp::Contains(std::string &input, const char *txt) {
  return _impl->contains(input, txt);
}

void ChecksumConversionApp::ProcessQBC(std::string &input,
                                       std::string *output) {
  _impl->replaceQBKeys(input, output);
}
}