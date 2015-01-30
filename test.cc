#include <unistd.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>

using std::string;
using std::vector;

const char kTempFileTemplate[] = "/tmp/lines_XXXXXX";

// Handy way to make some runtime assertions, and display a nice error
// message if they fail.
#define CHECK(x) \
    do {\
      if (x) break;\
      fprintf(stderr, "%s:%d: CHECK(%s) failed.\n",\
              __FILE__, __LINE__, #x);\
      exit(1);\
    } while (false)

// Returns an interpolation of Array[x], where x is a fractional number.
double Interpolate(const vector<double>& data, double pos) {
  if (round(pos) == pos) return data[static_cast<int>(pos)];
  const double flo = floor(pos);
  const int lo = static_cast<int>(flo);
  return (pos - flo) * data[lo] + (flo + 1 - pos) * data[lo + 1];
}

double Percentile(const vector<double>& data, double percentile) {
  CHECK(percentile >= 0);
  CHECK(percentile <= 100);
  CHECK(!data.empty());
  return Interpolate(data, percentile * 0.01 * (data.size() - 1));
}

// GenerateUniqueString() will always generate exactly the same string for
// a given seed.
void GenerateUniqueString(int seed, string* out) {
  out->clear();
  // Append the seed in base 26, reversed, followed by '@'. This guarantees unicity.
  for (int i = seed; i != 0; i /= 26) *out += char('a' + i % 26);
  *out += '@';
  // Append some random characters.
  srandom(seed);
  const int length = random() % 20;
  const int alphabet = 1 + random() % 26;
  for (int i = 0; i < length; ++i) *out += char('a' + random() % alphabet);
}

// Returns the name of the temporary file created.
string WriteNLinesWithMDistinctToTmpFile(int num_lines, int num_distinct_lines) {
  CHECK(num_distinct_lines <= num_lines);
  // Create the temporary file.
  char file_name[99];
  strcpy(/*dst=*/file_name, /*src=*/kTempFileTemplate);
  const int file_descriptor = mkstemp(file_name);
  FILE* file = fdopen(file_descriptor, "w");
  CHECK(file != NULL);

  // Special case: zero lines of output needed.
  if (num_distinct_lines == 0) {
    CHECK(num_lines == 0);
    fclose(file);
    return file_name;
  }

  // Initialize the seed deterministically, depending on 'num_lines' and
  // 'num_distinct_lines'.
  srandom(num_lines * 0xdeadbeef + num_distinct_lines);

  // Start by generating "num_distinct_lines" different line seeds. We do that
  // by picking 30 + 2 * num_distinct_lines random numbers, which should
  // guarantee that we obtain num_distinct_lines distinct ones after removing
  // duplicates, with ultra-strong probability.
  vector<int> line_seeds;
  line_seeds.reserve(num_lines);
  for (int i = 0; i < 30 + 2 * num_distinct_lines; ++i) {
    line_seeds.push_back(random());
  }
  std::sort(line_seeds.begin(), line_seeds.end());
  line_seeds.erase(std::unique(line_seeds.begin(), line_seeds.end()),
                   line_seeds.end());
  CHECK(int(line_seeds.size()) >= num_distinct_lines);
  std::random_shuffle(line_seeds.begin(), line_seeds.end());
  line_seeds.resize(num_distinct_lines);

  // Then re-pick existing line seeds until we reach num_lines. We choose
  // the "preferential pick" method, by re-picking uniformly at random from
  // the set of line seeds built so far. This will generate a power-law
  // distribution for the number of occurences of each line seed, which is
  // highly heterogeneous, and is a better stress-test than an homogeneous
  // distribution.
  while (int(line_seeds.size()) < num_lines) {
    line_seeds.push_back(line_seeds[random() % line_seeds.size()]);
  }

  // Re-shuffle.
  std::random_shuffle(line_seeds.begin(), line_seeds.end());

  // Finally, output the lines to the temp file.
  string line;
  for (int i = 0; i < int(line_seeds.size()); ++i) {
    GenerateUniqueString(line_seeds[i], &line);
    fputs(line.c_str(), file);
    fputc('\n', file);
  }
  CHECK(fclose(file) == 0);
  return string(file_name);
}

vector<double> RunExternalCommandToEstimateLineCardinality(
    int time_limit_seconds, const string& command, int cardinality) {
  const time_t absolute_deadline = time(NULL) + time_limit_seconds;
  double num_lines = cardinality;
  
  // Create a temporary file to hold the result of the external command.
  string result_file_name;
  {
    char tmp[99];
    strcpy(/*dst=*/tmp, /*src=*/kTempFileTemplate);
    const int file_descriptor = mkstemp(tmp);
    close(file_descriptor);
    result_file_name = tmp;
  }

  // Run the external command until the time limit is expired, gradually
  // increasing the number of lines.
  vector<double> estimates;
  do {
    const string tmp_filename =
        WriteNLinesWithMDistinctToTmpFile(int(num_lines), cardinality);
    const string command_line = command + " < " + tmp_filename + " > " +
        result_file_name;
    const int exit_code = system(command_line.c_str());
    if (exit_code != 0) {
      fprintf(stderr, "Your program failed on input '%s'. Command line: %s\n",
              tmp_filename.c_str(), command_line.c_str());
      return vector<double>();
    }
    double x = -1;
    FILE* result_file = fopen(result_file_name.c_str(), "r");
    CHECK(result_file != NULL);
    const int num_numbers_read = fscanf(result_file, "%lf", &x);
    CHECK(fclose(result_file) == 0);
    if (num_numbers_read != 1 || x != x) {
      fprintf(stderr,
              "Your program did not output a readable cardinality estimate on"
              " input '%s'.\nSee your program's output in '%s'. It must be a"
              " single number.\nFull command line: %s\n",
              tmp_filename.c_str(), result_file_name.c_str(), command_line.c_str());
      return vector<double>();
    }
    estimates.push_back(x);
    if (num_lines != 0) num_lines = num_lines * 1.1 + 1;

    // Clean up the temporary input file.
    CHECK(std::remove(tmp_filename.c_str()) == 0);
  } while (time(NULL) < absolute_deadline);

  // Clean up the temporary output file.
  CHECK(std::remove(result_file_name.c_str()) == 0);
  return estimates;
}

bool TestExternalCommandToEstimateCardinality(const string& command) {
  const int kNumTests = 8;
  const int kCardinalities[kNumTests] =
      {0, 1, 10, 100, 1000, 10000, 100000, 1000000};
  const int kTimeLimits[kNumTests] = {3, 3, 5, 10, 10, 15, 20, 30};
  int num_biased_estimates = 0;
  for (int i = 0; i < kNumTests; ++i) {
    const int cardinality = kCardinalities[i];
    fprintf(stderr,
            "\nRunning tests for cardinality %d (during up to %d seconds,"
            " unless your program is slower)...\n",
            cardinality, kTimeLimits[i]);
    vector<double> estimates =
        RunExternalCommandToEstimateLineCardinality(
            kTimeLimits[i], command, cardinality);
    if (estimates.empty()) {
      fprintf(stderr, "Your program failed. Test aborted.\n");
      return false;
    }
    if (estimates.size() < 5 && cardinality < 1000000) {
      fprintf(stderr,
              "    WARNING: your program was slow. It only managed to make"
              " %lu runs. The test result won't be trustworthy at all.\n",
              estimates.size());
    } else if (estimates.size() < 50) {
      fprintf(stderr,
              "    INFO: your program was okay, and did"
              " %lu runs. The test result won't be super accurate.\n",
              estimates.size());
    } else {
      fprintf(stderr,
              "    INFO: your program was fast -- it did %lu runs!"
              " The test result will be quite trustworthy.\n",
              estimates.size());
    }
    std::sort(estimates.begin(), estimates.end());
    const double median = Percentile(estimates, 50);
    const double perc5 = Percentile(estimates, 5);
    const double perc95 = Percentile(estimates, 95);
    fprintf(stderr, "  Median:%lf  [5-%%ile, 95-%%ile]: [%lf, %lf]\n",
	    median, perc5, perc95);
    const double adjusted_relative_error =
	cardinality == 0 ? (median == 0 ? 0 : 1) :
	fabs((median - cardinality) * sqrt(estimates.size()) / cardinality);
    if (adjusted_relative_error > 0.5) {
      fprintf(stderr,
	      "ERROR: your cardinality estimator seems very wrong:"
	      " expected %d, got a median of %lf (over %lu runs)\n",
	      cardinality, median, estimates.size());
      return false;
    }
    if (adjusted_relative_error > 0.1) {
      fprintf(stderr, "WARNING: your cardinality estimator seems imprecise: "
	      " expected %d, got a median of %lf (over %lu runs)\n",
	      cardinality, median, estimates.size());
      ++num_biased_estimates;
      if (num_biased_estimates >= 2) {
	fprintf(stderr, "ERROR: too many imprecise estimates.\n");
	return false;
      }
    }
  }
  fprintf(stderr,
	  "\nSUCCESS!\nYour program seems to be a good cardinality estimator!\n");
  return true;
}

int main(int argc, char** argv) {
  if (argc == 1) {
    fprintf(stderr,
	    "Usage: %s %%command that runs your hyperloglog program%%\n",
	    argv[0]);
    return EXIT_FAILURE;
  }
  string command;
  for (int i = 1; i < argc; ++i) {
    if (i > 1) command += " ";
    command += argv[i];
  }
  const bool ok = TestExternalCommandToEstimateCardinality(command);
  return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
