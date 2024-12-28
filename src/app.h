#pragma once

#define GHCHECKSUMS_APP
#include <ghchecksum.h>

#include <cstdio>
#include <memory>
#include <string>

using namespace GHChecksum;

constexpr const char *DEFAULT_OUTPUT = "output.q";

namespace ChecksumApp {
class ChecksumApp {
public:
  enum class RunMode { NONE, GEN_TABLE, GEN_TABLE_ROQ, TRANSLATE };

public:
  inline ChecksumApp(void) {}
  inline ~ChecksumApp(void) { _app.reset(); }

  inline int Run(int argc, char **argv) {
    _outputStrName.Set("%s", DEFAULT_OUTPUT); // Default name of output QBC file
    _app = std::make_unique<GHChecksumLib>(argc, argv);
    RunMode mode = RunMode::NONE;
    // Determine run type, depending on the command line arguments

    if (_app->Contains(argv[1], ".q") &&
             _app->Contains(argv[2], ".checksums")) {
      if (argv[3] != nullptr) { // Arg 3 has something (used for output name)
        _outputStrName.Set("%s", argv[3]);
      }
      mode = RunMode::TRANSLATE;
    } // Translate mode
    else if (_app->Contains(argv[1], ".txt")) // Generate table (ROQ script included)
      mode = RunMode::GEN_TABLE_ROQ;
    else if (_app->Contains(argv[1], ".q")) // Generate table
      mode = RunMode::GEN_TABLE;

    // Do different things depending on run mode
    std::string outputFile;
    switch (mode) {
    case RunMode::GEN_TABLE:
      _app->LoadText(argv[1], &_qbcScript);
      _app->GetChecksums(_qbcScript, &_qbkeyTable);
      outputFile = argv[1];
      outputFile =
          outputFile.substr(0, outputFile.find_last_of(".")) + ".checksums";
      _app->WriteText(_qbkeyTable, outputFile.c_str());

      printf("Checksums written to %s.\n\n", outputFile.c_str());
      return 0;
    case RunMode::GEN_TABLE_ROQ:
      _app->LoadText(argv[1], &_roqScript);
      if (_app->IsROQScript(_roqScript)) { // Make sure .txt file is an ROQ script before proceeding
        _app->GetROQValues(_roqScript); // Get ROQ values

        _app->OutputChecksums(&_qbkeyTable);

        outputFile = argv[1];
        outputFile =
            outputFile.substr(0, outputFile.find_last_of(".")) + ".checksums";
        _app->WriteText(_qbkeyTable, outputFile.c_str());
        return 0;
      }

      printf("File '%s' is not a ROQ script. Exiting.\n", argv[1]);
      break; // Should go to 'Return -1'
    case RunMode::TRANSLATE:
      _app->LoadText(argv[1], &_qbcScript);
      _app->LoadText(argv[2], &_qbkeyTable);

      _app->ParseChecksumsFile(_qbkeyTable);
      _app->ProcessQBC(_qbcScript, &_outputStr);

      _app->WriteText(_outputStr, _outputStrName.Get());
      return 0;
    default:
      _app->UsageMsg();
      return 0;
    }

    // If somehow we ended up here [probably due to an error], we'll return -1
    // to indicate an error
    return -1;
  }

private:
  std::unique_ptr<GHChecksumLib> _app;

  String _outputStrName;
  String _roqScript, _qbcScript, _qbkeyTable, _outputStr;
};

} // namespace ChecksumApp