#include "app.h"

int main(int argc, char **argv) {
  // Create a new instance of the application and assign it to the pointer
  std::unique_ptr <ChecksumApp::ChecksumApp> app = std::make_unique<ChecksumApp::ChecksumApp>();
  int result = app->Run(argc, argv);

  // Remove instance and deallocate pointer
  app.reset();
  return result;
}