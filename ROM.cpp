#include "ROM.hpp"
#include "INES.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>

ROM::ROM(std::string filename) : filename(filename) {
  std::ifstream file(filename, std::ios::binary);

  // get the header
  for (int_fast8_t i=0; i<16; i++) header.v1.header[i] = file.get();

  // deal with any errors
  FileErrorType error = VerifyROM(file);
  if (error != FileErrorType::OK) file.close();
  HandleFileError(error);

  file.seekg(16); // move file pointer past header

  // if there's only one unit of program ROM
  if (header.v1.prgromunits == 1) {
    memory = (byte *)malloc(2 * 0x4000);
    for (int i=0; i<0x4000; i++) {
      memory[i] = file.get();
      memory[i+0x4000] = memory[i];
    }

    file.close();
    return;
  }
  
  // if there's more than one unit of program ROM
  memory = (byte *)malloc(header.v1.prgromunits * 0x4000);
  for (int i=0; i<header.v1.prgromunits * 0x4000; i++) {
    memory[i] = file.get();
  }

  file.close();
}

ROM::FileErrorType ROM::VerifyROM(std::ifstream &file) {
  // if file failed to open, return the appropriate error
  if (file.fail()) return FileErrorType::CannotOpenFile;

  // compare the first 4 bytes to their expected values for a .nes file, and 
  // return the appropriate error if they differ
  byte expected_nes[4] = {'N', 'E', 'S', 0x1a};
  if (memcmp(header.v1.NES, expected_nes, 4)) return FileErrorType::IncorrectFiletype;
  
  // find size of file and compare with proper size
  // return the appropriate error if they differ
  int expected_filesize = 16 + header.v1.prgromunits*0x4000 + header.v1.chrromunits*0x2000;
  file.seekg(0); // seek to start of file
  int start = file.tellg();
  file.seekg(0, std::ios::end); // seek to start of file
  int end = file.tellg();

  if (expected_filesize != end-start) return FileErrorType::IncorrectLength;

  // if not, return OK
  return FileErrorType::OK;
}

void ROM::HandleFileError(FileErrorType err) {
  switch (err) {
    case FileErrorType::IncorrectFiletype:
      throw std::logic_error("Incorrect file type!");
      break;
    case FileErrorType::CannotOpenFile:
      throw std::logic_error("File cannot be opened!");
      break;
    case FileErrorType::IncorrectLength:
      throw std::logic_error("File is incorrect length!");
      break;
    case FileErrorType::OK:
      break;
  }
}
