// standard cpp library
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <list>

// POSIX APIs (linux variant)
#include <spawn.h>
#include <sys/wait.h>

// 3rd party library
#include "scheduler/json.hpp"

using json = nlohmann::basic_json<nlohmann::ordered_map>;
static json config_db, result_db;
std::map<std::string, int> var_db;
char arg_pool[32][64];   // the maximal is 32 64-byte long arguments
char * gargv[33];

// global configure parameters
bool debug_run = false;


// json related functions
bool read_json(json &db, const std::string& fn, bool notice);
bool dump_json(json &db, const std::string& fn, bool notice);

// test case related
extern char **environ; // especially required by spawn
std::list<std::string> collect_case_list();
typedef std::list<std::string> str_list_t;
typedef std::list<str_list_t>  str_llist_t;
int case_parser(const std::string& cn, str_list_t& arg_list, std::string& vn, std::set<int> &results);
char ** argv_conv(const std::string &cmd, const str_list_t &args);
int run_cmd(const char *argv[]);
bool run_tests(std::list<std::string> cases);

int main(int argc, char* argv[]) {
  // parse argument
  for(int i=1; i<argc; i++) {
    if(std::string(argv[i]) == "debug") debug_run = true;
  }

  // read the configure file
  if(!read_json(config_db, "configure.json", false)) return 1;

  run_tests(collect_case_list());

  return 0;
}

bool read_json(json &db, const std::string& fn, bool notice) {
  std::ifstream db_file(fn);
  if(db_file.good()) {
    if(notice) std::cout << "read json file " << fn << std::endl;
    db_file >> db;
    db_file.close();
  } else {
    std::cerr << "Fail to open json file `" << fn << "'" << std::endl;
    return false;
  }
}

bool dump_json(json &db, const std::string& fn, bool notice) {
  std::ofstream db_file(fn);
  if(!db_file.fail()) {
    if(notice) std::cout << "dump json " << fn << std::endl;
    db_file << db.dump(4);
    db_file.close();
    return true;
  } else {
    std::cerr << "Fail to dump json file `" << fn << "'" << std::endl;
    return false;
  }
}

std::list<std::string> collect_case_list() {
  std::list<std::string> rv;
  for(auto record: config_db.get<std::map<std::string, json> >())
    rv.push_back(record.first);
  return rv;
}

int case_parser(const std::string& cn, std::string& pn, str_llist_t& arg_list, std::string& vn,
                 std::set<int> &expect_results, std::set<int> &retry_results) {
  // check whether the case exist
  if(!config_db.count(cn)) {
    std::cerr << "Fail to parse test case " << cn << std::endl;
    return -1;
  }

  auto tcase = config_db[cn];

  // check requirement
  if(tcase.count("require")) {
    auto require_list = tcase["require"].get<str_llist_t>();
    for(auto and_conds : require_list)
      for(auto or_cond : and_conds)
        if(!result_db.count(or_cond))
          return 1;
    for(auto and_conds : require_list) {
      bool can_test = false;
      for(auto or_cond : and_conds) {
        if(result_db.count(or_cond) && 0 == result_db[or_cond]["result"].get<int>())
          can_test = true;
      }
      if(!can_test) return 1024;
    }
  }

  // get the program name
  if(tcase.count("program"))
    pn = tcase["program"].get<std::string>();
  else
    pn = cn;

  // get the argument lists
  arg_list.clear();
  arg_list.push_back(str_list_t());
  if(tcase.count("arguments")) {
    auto arguments = tcase["arguments"].get<str_list_t>();
    for(auto arg : arguments) {
      if(arg.size() >= 3) {
        auto atype = arg.substr(0,2);
        if(atype == "-r") { // range
          auto range_name = arg.substr(2);
          if(tcase.count(range_name)) {
            auto range_value = tcase[range_name].get<std::vector<int> >();
            auto arg_list_size_org = arg_list.size();
            for(int i=0; i<arg_list_size_org; i++) {
              auto ale = arg_list.front();
              arg_list.pop_front();
              for(int r=range_value[0]; r<range_value[1]; r+=range_value[2]) {
                auto new_ale = str_list_t(ale);
                new_ale.push_back(std::to_string(r));
                arg_list.push_back(new_ale);
              }
            }
          } else // fallback
            for(auto &ale : arg_list) ale.push_back(arg);
        } else if(atype == "-l") { // list
          auto list_name = arg.substr(2);
          if(tcase.count(list_name)) {
            auto list_value = tcase[list_name].get<std::vector<int> >();
            auto arg_list_size_org = arg_list.size();
            for(int i=0; i<arg_list_size_org; i++) {
              auto ale = arg_list.front();
              arg_list.pop_front();
              for(auto v: list_value) {
                auto new_ale = str_list_t(ale);
                new_ale.push_back(std::to_string(v));
                arg_list.push_back(new_ale);
              }
            }
          } else // fallback
            for(auto &ale : arg_list) ale.push_back(arg);
        } else if(atype == "-v") { // variable
          auto var_name = arg.substr(2);
          if(var_db.count(var_name))
            for(auto &ale : arg_list) ale.push_back(std::to_string(var_db[var_name]));
          else // fallback
            for(auto &ale : arg_list) ale.push_back(arg);
        }
        else // other, fallback
          for(auto &ale : arg_list) ale.push_back(arg);
      } else
        for(auto &ale : arg_list) ale.push_back(arg);
    }
  }

  // check whether the case store a global variable
  if(tcase.count("set-var")) {
    vn = tcase["set-var"].get<std::string>();
  } else
    vn.clear();

  // collect the known exit code other than 0
  expect_results.clear();
  if(tcase.count("expect-results")) {
    for(auto r: tcase["expect-results"].get<std::map<std::string, json> >()) {
      expect_results.insert(std::stoi(r.first));
    }
  }
  retry_results.clear();
  if(tcase.count("retry-results")) {
    for(auto r: tcase["retry-results"].get<std::map<std::string, json> >()) {
      retry_results.insert(std::stoi(r.first));
    }
  }

  return 0;
}

int run_cmd(char *argv[]) {
  pid_t pid;
  int rv = posix_spawnp(&pid, argv[0], NULL, NULL, argv, environ);
  if(rv) {
    if(rv == ENOSYS) {
      std::cerr << "posix_spawn() is support in this system!" << std::endl;
      exit(1);
    } else {
      return rv;
    }
  }

  int s, status;
  do {
     s = waitpid(pid, &status, WUNTRACED | WCONTINUED);
     if(s == -1) {
       std::cerr << "waitpid() is support in this system!" << std::endl;
       exit(1);
     }
  } while (!WIFEXITED(status) && !WIFSIGNALED(status));

  if(WIFSIGNALED(status)) {
    std::cout << argv[0] << " terminated with signal " << WIFSIGNALED(status) << std::endl;
    return 256+WIFSIGNALED(status);
  }
  if(WIFEXITED(status))
    return WEXITSTATUS(status);
}

char ** argv_conv(const std::string &cmd, const str_list_t &args) {
  int i = 0;
  strcpy(arg_pool[i++], cmd.c_str());
  for(const auto a:args) strcpy(arg_pool[i++],a.c_str());
  for(int j=0; j<i; j++) gargv[j]=arg_pool[j];
  gargv[i] = NULL;
  return gargv;
}


bool run_tests(std::list<std::string> cases) {
  std::string prog, cmd;
  str_llist_t alist;
  std::string gvar;
  std::set<int> expect_results, retry_results, rvs;
  while(!cases.empty()) {
    auto cn = cases.front();
    cases.pop_front();
    int test_cond = case_parser(cn, prog, alist, gvar, expect_results, retry_results);
    if(test_cond == 0) {
      std::cout << "\n========== " << cn << " =========" << std::endl;
      std::cout << "make test/" << prog << std::endl;
      int rv = 0;
      int index = 0;
      rvs.clear();
      rv = run_cmd(argv_conv("make", str_list_t(1, "test/" + prog)));
      if(rv){
        std::cout << "fail to make " << prog << " with error status " << rv << std::endl;
        rv = -1;
      } else { // run the test case
        for(auto arg:alist) {
          cmd = "test/" + prog;
          std::cout << "\n" << cmd; for(auto a:arg) std::cout << " " << a; std::cout << std::endl;
          rv = run_cmd(argv_conv(cmd, arg));
          if(0 == rv) break;
          else {
            rvs.insert(rv);
            index++;
          }
        }
        if(rv != 0) {
          for(auto v:rvs) {
            if(expect_results.count(v)) rv = v;
            else if(!retry_results.count(v)) rv = v;
          }
        }
      }

      // record
      result_db[cn]["result"] = rv; dump_json(result_db, "results.json", false);
      if(!gvar.empty()) {
        std::cerr << "set runtime variable " << gvar << " to " << index << std::endl;
        var_db[gvar] = index;
      }
      if(rv != 0 && !expect_results.count(rv)) {
        std::cerr << "Test abnormality: " << cn << " failed with unexpected exit value " << rv << std::endl;
        if(debug_run) exit(1);
      }
    } else if(test_cond == 1)
      cases.push_back(cn);
    else if(test_cond == -1) {
        std::cerr << "Test abnormality: " << cn << " does not exist in the configure.json file." << std::endl;
        if(debug_run) exit(1);      
    } else {
      result_db[cn]["result"] = test_cond; dump_json(result_db, "results.json", false);
    }
  }
  return true;
}
