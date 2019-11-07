#include <iostream>
#include "tree.h"
#include <unordered_map>
// coding

int main(int argc, char* argv[]) {
    if (argc == 1){
        std::cout << "No arguments\n";
        return 0;
    }
    std::unordered_map<std::string, std::string> args;
    args["-o"] = "out";
    for (int i = 1; i < argc; i += 2){
        args[argv[i]] = argv[i+1];
    }
    std::string file;
    if (!args.count("-f")){
        std::cout << "Use -f flag to chose file";
        return 0;
    }
    std::ifstream fin(args["-f"]);
    char c;
    std::string s;
    while (getline(fin, s)) {
        file += s;
        file += '\n';
    }
    std::cerr << file << '\n';
    file.pop_back();
    HTree t(file);
    auto dmp = t.dump();
    auto res = t.toCode(file);
    std::ofstream dmpout(args["-o"] + ".tree"), resout(args["-o"] + ".myrar");
    for (int i = 0; i < dmp.size(); i++)
            dmpout << int(dmp[i].first) << ' ' << dmp[i].second << '\n';
    std::cerr<<"\n\n";
    // std::cout << args["-0"] + ".tree" << ' ' << resout << '\n';
    resout << file.size() << ' ' << res.size() << '\n';
    std::cerr<<"\n\n";
    for (int i = 0; i < res.size(); i++) {
        resout << res[i];
        std::cerr << res[i];
    }
}