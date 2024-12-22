#include "app.h"

int main(int argc, char **argv) {
  // Create a new instance of the application and assign it to the pointer
  std::unique_ptr <ChecksumApp::ChecksumApp> app = std::make_unique<ChecksumApp::ChecksumApp>();
  int result = app->Run(argc, argv);

  // Remove instance and deallocate pointer
  app.reset();
  return result;
}

/* Old code */

//#include <ghchecksum.h>
//
//#include <cstdio>
//
//using namespace GHChecksum;
//
//int main(int argc, char **argv) {
//  ChecksumConversionApp app(argc, argv);
//
//  if (argc < 2 || argc > 4) {
//    app.UsageMsg();
//  } else if (argc < 3) { // Expecting .q file
//    std::string qScript;
//    if (app.Contains(std::string(argv[1]), ".q")) {
//      app.LoadText(argv[1], &qScript);
//      std::string checksums = app.GetChecksums(qScript);
//      std::string output = std::string(argv[1]);
//      output = output.substr(0, output.find_last_of(".")) + ".checksums";
//
//      app.WriteText(checksums, output.c_str());
//      printf("Checksums written to %s.\nAppend each QBKey with their original value, separated with two spaces.\n\n", output.c_str());
//      printf("Example:\n#\"0xf7d18982\"  Hello\n#\"0xfbb63e47\"  World\n\n");
//      return 0;
//    }
//    printf("Not a QBC script. Exiting");
//  } else if (argc < 4) { // .checksums file, then .q file
//    if (app.Contains(std::string(argv[1]), ".q") &&
//        app.Contains(std::string(argv[2]), ".checksums")) {
//      std::string qScript, checksumList, output;
//
//      // Load QBC script and checksums table
//      app.LoadText(argv[1], &qScript);
//      app.LoadText(argv[2], &checksumList);
//
//      // Parse checksums table
//      app.ParseChecksumsFile(checksumList);
//
//      /*
//      Convert QBKey checksums inside QBC script back into
//      their original values, using the _checksumList unordered_map
//      */
//      app.ProcessQBC(qScript, &output);
//
//      app.WriteText(output, "output.q");
//
//      return 0;
//    }
//    printf("Incorrect inputs. Exiting (lack of a better message)");
//  }
//
//
//  //std::string fileName = argv[1];
//
//  //std::string file;
//  //app.LoadText(fileName.c_str(), &file);
//
//  //if (app.Contains(fileName, ".checksums")) {
//  //  app.ParseChecksumsFile(file);
//  //} else if (app.Contains(fileName, ".q")) {
//  //  std::string checksums = app.GetChecksums(file);
//
//  //  std::string outputQBName = argv[1];
//  //  outputQBName += ".checksums";
//
//  //  app.WriteText(checksums, outputQBName.c_str());
//  //} else {
//  //  printf("Exiting. Didn't input a QBC or checksums file\n");
//  //}
//  return 0;
//}