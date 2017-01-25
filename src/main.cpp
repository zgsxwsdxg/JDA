#include <cstdio>
#include <cstring>

using namespace std;

void train();
void test();
void resume();
void live();
void fddb(const char* model_file);
void dump();

/*! \brief command help */
static const char help[] = "Joint Cascade Face Detection and Alignment\n\n"
                           "train:  train JDA classifier and regressor for face detection\n"
                           "        and face alignemnt\n"
                           "test:   test over your own data\n"
                           "resume: resume a previous training status\n"
                           "live:   live demo with camera support\n"
                           "fddb:   detection over fddb\n\n";

/*!
 * \brief Command Dispatch
 */
int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf(help);
  }
  else if (strcmp(argv[1], "train") == 0) {
    train();
  }
  else if (strcmp(argv[1], "test") == 0) {
    test();
  }
  else if (strcmp(argv[1], "resume") == 0) {
    resume();
  }
  else if (strcmp(argv[1], "live") == 0) {
    live();
  }
  else if (strcmp(argv[1], "fddb") == 0) {
    fddb(argv[2]);
  }
  else if (strcmp(argv[1], "dump") == 0) {
    dump();
  }
  else {
    printf(help);
  }
  return 0;
}
