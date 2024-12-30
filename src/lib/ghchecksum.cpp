#include "ghchecksum.h"

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/* Pattern used in the library to help find QBKeys [ Format is #"0x00000000" -
 * #"0xffffffff" ] */
constexpr const char *QB_PATTERN = R"(\#\"0x[0-9A-Fa-f]+\")";

namespace GHChecksum {

/* -------------------------------------------------- */
/* GHChecksumLib - Private Implementation */
/* -------------------------------------------------- */

class GHChecksumLib::GHChecksumLib_Impl {
public:
  GHChecksumLib_Impl(int argc, char **argv) {
    // Only proceed if command-line arguments are inputted
    if (argc != 0 && argv != nullptr) {
      _convertedArgs.clear();
      for (int i = 0; i < argc; ++i) {
        _convertedArgs.push_back(argv[i]);
      }
    }
  }
  ~GHChecksumLib_Impl(void) {}

  void usageMsg(void) { printf("----- GHChecksum Usage -----\nTo generate a checksums table:\n(QBC Scripts): GHChecksum [.q]\n(ROQ Scripts): GHChecksum [.txt]\n\nTo translate QBKeys in QBC script:\nGHChecksum [.q] [.checksums] [Output .q file, optional]\n");
  }

  bool hasArg(const char *arg, usize *foundArgIndex) {
    std::string arg_toCompare = arg;
    for (usize i = 1; i < _convertedArgs.size(); ++i) {
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
    if (output != nullptr)
      output->Clear();

    std::string strInput = input.Get();

    std::regex pattern(QB_PATTERN);
    auto matches_begin =
        std::sregex_iterator(strInput.begin(), strInput.end(), pattern);

    auto matches_end = std::sregex_iterator();

    std::vector<std::string> uniqueMatches;
    std::unordered_set<std::string> seen;

    // Index 0 is total, Index 1 is unique matches
    int counter[2] = {0,0};
    for (std::sregex_iterator it = matches_begin; it != matches_end; ++it) {
      counter[0]++;
      std::string match = it->str();
      if (seen.insert(match).second) {
        uniqueMatches.push_back(match);
      }
    }

    
    // Output all unique matches to output string
    for (const auto &match : uniqueMatches) {
      // strOutput += match + '\n';
      counter[1]++;
      std::string hex = match.substr(
          2, match.size() - 3); // Remove the #" prefix and " suffix
      printf("%s\n", hex.c_str());

      if (output != nullptr)
        output->Append("%s\n", hex.c_str());

      // Parse to u32 and push to _qbList vector
      try {
        u32 hexProper = std::stoul(hex, nullptr, 16);
        _qbcChecksums.push_back(hexProper);
      } catch (std::exception &ex) {
        printf("Exception caught: %s\nSkipping entry %s", ex.what(),
               hex.c_str());
        continue;
      }
    }
    printf("%d QBKeys found in script [ %d unique QBKeys ]\n\n", counter[0],
           counter[1]);    
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
      usize separator = line.find("  "); // Two spaces
      if (separator != std::string::npos) {
        std::string qbKey = line.substr(0, separator);
        std::string origValue = line.substr(separator + 2);
        // _checksumList[qbKey] = origValue;

        // Using the new checksums table for this
        u32 hex = std::stoul(qbKey, nullptr, 16);
        u32 hexCheck = QBKeyFromString(origValue.c_str());
        if (hexCheck == hex) { // Make sure the QBKey matches the original value
          _checksumsTable[hex] = origValue;
          continue;
        }
        
        printf("WARNING: CRC32 of value '%s' is incorrect. Skipping.\n[ QBKey for '%s' is supposed to be 0x%08x, got 0x%08x instead. ]\n\n", origValue.c_str(), origValue.c_str(), hex, hexCheck);
      }
    }
  }

  void replaceQBKeys(String &input, String *output) {
    output->Clear();
    int counter = 0;
    std::string qbc = input.Get();

    for (const auto &pair : _checksumsTable) {
      const String qbToString("#\"0x%08x\"", pair.first);
      const std::string &QBKey = qbToString.Get(); // #"0x[Hex]"
      const std::string &OrigValue = pair.second;

      usize pos = 0;
      while ((pos = qbc.find(QBKey, pos)) != std::string::npos) {
        qbc.replace(pos, QBKey.length(), OrigValue);
        pos += OrigValue.length();

        counter++;
      }
    }
    printf("Processed %d QBKeys\n\n", counter);
    output->Set("%s",
                qbc.c_str()); // Set contents of output to the processed QBC
  }

  // Generate and output CRC32 checksum
  u32 QBKeyFromString(const char *input) {
    // Using C++ string to make things a little easier
    std::string strInput(input);

    // Ensure the string is in lower-case before proceeding
    std::for_each(strInput.begin(), strInput.end(),
                  [](char &c) { c = tolower(c); });

    // If input is blank, return 0x00
    if (strInput == "") {
      return 0x00000000;
    }

    u32 crc = 0xFFFFFFFF;
    for (char chr : strInput) {
      u8 byte = static_cast<u8>(
          chr); // Convert to 8-bit integer, unsigned (unsigned char)
      u32 index = (crc ^ byte) & 0xFF;
      crc = _crcTable[index] ^ (crc >> 8);
    }
    return crc;
  }

  void processROQ(String &input) {
    int counter[2] = {0,0};

    std::string strInput(input.Get());
    for (std::regex pat : _roqPatterns) {
      auto matches_begin =
          std::sregex_iterator(strInput.begin(), strInput.end(), pat);
      auto matches_end = std::sregex_iterator();

      std::vector<std::string> uniqueMatches;
      std::unordered_set<std::string> seen;
      for (std::sregex_iterator it = matches_begin; it != matches_end; ++it) {
        counter[0]++;
        std::string match = (*it)[1].str();
        if (seen.insert(match).second) {
          _roqList.push_back(match);
          uniqueMatches.push_back(match);
          counter[1]++;
        }
      }
    }

    // Add entries into checksums table
    for (usize i = 0; i < _roqList.size(); ++i) {
      u32 key = QBKeyFromString(_roqList[i].c_str());
      _checksumsTable[key] = _roqList[i];
    }

    printf("Found %d values in ROQ script [ %d unique values ]\n", counter[0],
           counter[1]);
  }

  /* Output .checksums table [QBKeys only] */
  void WriteTable_QBKeys(String *output) {
    if (output == nullptr) {
      printf("Error [GHChecksum->WriteTable_QBKeys]: 'output' string cannot be null.\n");
      return;
    }

    output->Clear();
    // Loop through each entry
    for (usize i = 0; i < _qbcChecksums.size(); ++i) {
      output->Append("0x%08x\n", _qbcChecksums[i]);
    }
  }

   /* Output .checksums table */
  void WriteTable(String *output) {
    if (output == nullptr) {
      printf("Error [GHChecksum->WriteTable]: 'output' string cannot be null.\n");
      return;
    }

    output->Clear();
    // Loop through each entry
    for (auto entry : _checksumsTable) {
      output->Append("0x%08x  %s\n", entry.first, entry.second.c_str());
    }
  }

  /* Form checksums table by comparing QBKeys with values from ROQ script */
  void AutoFillChecksums(void) {
    // Loop through each keyword
    for (usize i = 0; i < _roqList.size(); ++i) {
      // Loop through each QBKey, looking for a match
      for (usize j = 0; j < _qbcChecksums.size(); ++j) {
        std::string *currentKeyword = &_roqList[i];
        u32 keywordQB = QBKeyFromString(currentKeyword->c_str());

        if (keywordQB == _qbcChecksums[j]) { // Generate checksum for current keyword, then compare
          _checksumsTable[keywordQB] = *currentKeyword;
        }
      }
    }
  }

  // Is .txt file an ROQ script?
  bool isROQ(String &input) {
    std::string strInput = input.Get();
    std::regex pattern(R"(Unknown\s+\[GHWT\_HEADER\])");
    auto matches_begin =
        std::sregex_iterator(strInput.begin(), strInput.end(), pattern);
    auto matches_end = std::sregex_iterator();
    for (std::sregex_iterator it = matches_begin; it != matches_end; ++it) {
      return true;
    }
    return false;
  }

private:
  std::vector<std::string> _convertedArgs;
//  std::unordered_map<std::string, std::string> _checksumList; // Old


  // CRC32 table used for Tony Hawk/Guitar Hero engines
  const u32 _crcTable[256] = {
      0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
      0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
      0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
      0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
      0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
      0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
      0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
      0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
      0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
      0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
      0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
      0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
      0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
      0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
      0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
      0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
      0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
      0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
      0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
      0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
      0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
      0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
      0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
      0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
      0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
      0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
      0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
      0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
      0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
      0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
      0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
      0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
      0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
      0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
      0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
      0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
      0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
      0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
      0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
      0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
      0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
      0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
      0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d};

  std::vector<std::regex> _roqPatterns = {
      std::regex(R"(Script\s+(\S*))"),
      std::regex(R"(SectionStruct\s+(\S*))"),
      std::regex(R"(StructQBKey\s\S+\s*=\s*(\S*))"),
      std::regex(R"(SectionArray\s+(\S*))"),
      std::regex(R"(StructArray\s+(\S*))"),
      std::regex(R"($(\S*)$)"),
      std::regex(R"(\S*)")
  };
  // QBKeys list generated from reading the inputted QBC script
  std::vector<u32> _qbcChecksums;

  // ROQ Checksum table
  std::vector<std::string> _roqList;

  // Final checksums table is stored in here
  std::unordered_map<u32, std::string> _checksumsTable;
};

/* -------------------------------------------------- */
/* GHChecksumLib - Public Implementation */
/* -------------------------------------------------- */

#if defined(_WIN32) && !defined(GH_STATIC_LIB)
GHChecksumLib::GHChecksumLib(int argc, char **argv)
    : _impl(new GHChecksumLib_Impl(argc, argv)) {}
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

void GHChecksumLib::WriteText(String &input, const char *fileName) {
  _impl->writeTextFile(input, fileName);
}

void GHChecksumLib::ProcessQBC(String &input, String *output) {
  _impl->replaceQBKeys(input, output);
}

bool GHChecksumLib::LoadText(const char *filePath, String *outputStr) {
  return _impl->loadTextFile(filePath, outputStr);
}

bool GHChecksumLib::Contains(String &input, const char *txt) {
  return _impl->contains(input, txt);
}
bool GHChecksumLib::Contains(const char *input, const char *txt) {
  String strInput("%s", input); // Store input into String
  return _impl->contains(strInput, txt);
}

void GHChecksumLib::GetChecksums(String &input, String *output) {
  _impl->getChecksums(input, output);
}

u32 GHChecksumLib::GetQBKey(const char *input) {
  return _impl->QBKeyFromString(input);
}

void GHChecksumLib::GetROQValues(String &input) {
  _impl->processROQ(input);
}

bool GHChecksumLib::IsROQScript(String &input) { return _impl->isROQ(input); }

void GHChecksumLib::ProcessChecksums(void) { _impl->AutoFillChecksums(); }

void GHChecksumLib::OutputChecksums(String *output) {
  _impl->WriteTable(output);
}

void GHChecksumLib::OutputChecksums_QB(String *output) {
  _impl->WriteTable_QBKeys(output);
}
} // namespace GHChecksum