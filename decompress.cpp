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
    for (int i = 1; i + 1 < argc; i += 2){
        args[argv[i]] = argv[i+1];
    }
    std::cerr << args["-f"] << '\n';
    std::string file;
    if (!args.count("-f")){
        std::cout << "Use -f flag to chose file\n";
        return 0;
    }
    if (!args.count("-t")){
        std::cout << "Use -t flag to chose table file\n";
        return 0;
    }
    std::ifstream fin(args["-f"]);
    int n;
    fin >> n;
    char c;
    while ((c = fin.get()) != EOF)
        file += c;
    HTree t; // FIX ME
    std::cerr << n << ' ' << file << '\n';
    std::cerr << ("init file:" + args["-t"] + '\n');
    t.initByFile(args["-t"]);
    std::cerr << "kek4\n";
    std::vector<unsigned char> v(file.size());
    std::cerr << "v " << v.size() << '\n';
    for (int i = 0; i<file.size(); i++)
        v[i] = file[i];

    auto res = t.toStdString(v, n);
    std::ofstream  resout(args["-o"] + ".res");
        if (args["-o"].empty()){
            for (int i = 0; i < res.size(); i++)
                std::cout << res[i];  
            std::cerr << "Done\n";  
            return 0;        
        }
    for (int i = 0; i < res.size(); i++){
        resout << res[i];
        std::cerr << res[i];
    }
    std::cerr << "\nDone\n";
}