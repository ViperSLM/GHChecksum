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
  inline ChecksumApp(void) {}
  inline ~ChecksumApp(void) { _app.reset(); }

  inline int Run(int argc, char **argv) {
    _app = std::make_unique<GHChecksumLib>(argc, argv);

    if (argc < 2 || argc > 4) { /* Outcome 1: No arguments or too many arguments */
      _app->UsageMsg();
      return 0;
    } else if (argc < 3) { /* Outcome 2: Expecting a QBC script as the only input */
      String fileName("%s", argv[1]);
      if (_app->Contains(fileName, ".q")) {          // Make sure the file end with '.q' before proceeding
        _app->LoadText(fileName.Get(), &_qbcScript); // Load the specified file and store into _qbcScript string
        _app->GetChecksums(_qbcScript, &_qbkeyTable);

        // C++ string trickery because I'm too lazy to implement this in the custom string
        std::string temp(fileName.Get());

        /* Replace the '.q' suffix in the filename with '.checksums'.
           This will be the filename for the new table. */
        temp = temp.substr(0, temp.find_last_of(".")) + ".checksums"; // Replace the '.q' suffix in the filename
        _app->WriteText(_qbkeyTable, temp.c_str()); // Save checksums table to file

        printf("Checksums written to %s.\nAppend each QBKey with their original value, separated with two spaces.\n\n", temp.c_str());
        printf("Example:\n#\"0xf7d18982\"  Hello\n#\"0xfbb63e47\"  World\n\n");
        return 0;
      }
      // Incorrect input
      printf("Not a QBC script. Exiting\n[inputted file was %s]\n", fileName.Get());
    } else if (argc < 4) { /* Outcome 3: QBC script and checksums table as inputs */
      String input1("%s", argv[1]), input2("%s", argv[2]);

      // Make sure file extensions are correct before proceeding
      if (_app->Contains(input1, ".q") && _app->Contains(input2, ".checksums")) {
        // Load the QBC script and checksums table and store into their respective strings
        _app->LoadText(input1.Get(), &_qbcScript);
        _app->LoadText(input2.Get(), &_qbkeyTable);

        // Parse the checksums file
        _app->ParseChecksumsFile(_qbkeyTable);

        // Convert QBKey checksums back into their original values
        _app->ProcessQBC(_qbcScript, &_outputStr);

        // Write result into new file
        _app->WriteText(_outputStr, DEFAULT_OUTPUT);

        return 0;
      }
      // File extension check failed
      printf("Incorrect inputs. Exiting.\n[inputted files were %s and %s]\n", input1.Get(), input2.Get());
    }

    // If somehow we ended up here [probably due to an error], we'll return -1 to indicate an error
    return -1;
  }

private:
  std::unique_ptr <GHChecksumLib> _app;

  String _qbcScript, _qbkeyTable, _outputStr;
};

}