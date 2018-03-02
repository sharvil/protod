#include <cstdio>
#include <deque>
#include <set>
#include <string>
#include <vector>
#include <google/protobuf/compiler/importer.h>

using namespace google::protobuf;
using namespace google::protobuf::compiler;

static constexpr auto VERSION = "1.0.0";
static constexpr int STATUS_SUCCESS = 0;
static constexpr int STATUS_ERROR = 1;
static constexpr int STATUS_INFO = 2;

class ErrorCollector : public MultiFileErrorCollector {
  public:
    ErrorCollector() : errorCount(0) {}
    virtual ~ErrorCollector() = default;

    virtual void AddError(const std::string &filename, int line, int column, const std::string &message) {
      fprintf(stderr, "[ERR ] %s:%d,%d: %s\n", filename.c_str(), line, column, message.c_str());
      ++errorCount;
    }

    virtual void AddWarning(const std::string &filename, int line, int column, const std::string &message) {
      fprintf(stderr, "[WARN] %s:%d,%d: %s\n", filename.c_str(), line, column, message.c_str());
    }

    int errorCount;
};

static void AddDependencies(const FileDescriptor *desc, std::set<std::string> *result) {
  if (!desc)
    return;

  std::deque<const FileDescriptor *> open;
  std::set<const FileDescriptor *> closed;

  open.push_back(desc);
  while (!open.empty()) {
    const FileDescriptor *current = open.front();
    open.pop_front();
    result->insert(current->name());
    closed.insert(current);
    for (int i = 0; i < current->dependency_count(); ++i) {
      const FileDescriptor *child = current->dependency(i);
      if (closed.find(child) == closed.end())
        open.push_back(child);
    }
  }
}

static void Usage(const char *name) {
  printf("Usage: %s [OPTIONS] PROTO_FILES...\n", name);
  printf("Example: %s -I/usr/include -I/usr/local/include -I. google/example/library/v1/library.proto\n", name);
  printf("\n");
  printf("Options:\n");
  printf("  -h, --help    show this help message and exit\n");
  printf("  --version     show version info and exit\n");
  printf("  -Ipath        add to search path for .proto files\n");
  exit(STATUS_INFO);
}

int main(int argc, char **argv) {
  std::vector<std::string> inputFiles;
  std::vector<std::string> paths;

  for (int i = 1; i < argc; ++i)
    if (strstr(argv[i], "-I") == argv[i]) {
      // Handle both -I<path> and -I <path>
      if (argv[i][2] != '\0')
        paths.push_back(argv[i] + 2);
      else if ((i + 1) < argc)
        paths.push_back(argv[++i]);
    } else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
      Usage(argv[0]);
    } else if (!strcmp(argv[i], "--version")) {
      printf("%s version %s\n", argv[0], VERSION);
      return STATUS_INFO;
    } else {
      inputFiles.push_back(argv[i]);
    }

  if (inputFiles.empty())
    Usage(argv[0]);

  DiskSourceTree sourceTree;
  if (paths.empty()) {
    sourceTree.MapPath("", "/usr/include");
    sourceTree.MapPath("", "/usr/local/include");
    sourceTree.MapPath("", ".");
  } else {
    for (const auto &s : paths)
      sourceTree.MapPath("", s.c_str());
  }

  ErrorCollector errorCollector;
  Importer importer(&sourceTree, &errorCollector);

  std::set<std::string> closure;
  for (const auto &file : inputFiles)
    AddDependencies(importer.Import(file), &closure);

  if (errorCollector.errorCount && !closure.empty())
    fprintf(stderr, "[WARN] Errors found while parsing protobuf file(s). Displaying incomplete list of files:\n");

  for (const auto &file : closure) {
    std::string diskFile;
    if (sourceTree.VirtualFileToDiskFile(file, &diskFile))
      printf("%s\n", diskFile.c_str());
  }

  return (errorCollector.errorCount) ? STATUS_ERROR : STATUS_SUCCESS;
}
