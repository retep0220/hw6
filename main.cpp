//
//  main.cpp
//  HW6
//
//  Created by 王吉朗 on 2023/5/20.
//

#include <iostream>
#include "unordered_map"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "istream"
#include <vector>
#include "queue"

using namespace std;

class Sentence {
public:
    unordered_map<string, int> srored;
    int idf = 0;
    int id;
};

vector<string> tokenizer (string str) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

bool isCapital (char ch) {
    if(ch >= 'A' && ch <= 'Z') {
        return true;
    }
    else return false;
}

struct IsNotAlpha {
    bool operator()(char c) {
        return !std::isalpha(static_cast<unsigned char>(c));
    }
};

struct CompElement {
    bool operator()(const Sentence& a, const Sentence& b) {
        if (a.idf != b.idf) {
            return a.idf < b.idf;
        }
        return a.id > b.id;
    }
};

int main(int argc, const char * argv[]) {
    ifstream corpusFile(argv[1]);
    static int total_words = 0;
    string line;
    int k = atoi(argv[3]);
    vector<Sentence> AllSentence;
    while (getline(corpusFile,line)) {
        total_words++;
        string number;
        istringstream tempss(line);
        getline(tempss, number, ',');
        vector<string> tempstr;
        tempstr = tokenizer(line);
        for (string &str : tempstr) {
            str.erase(remove_if(str.begin(), str.end(), IsNotAlpha()),str.end()); //delete all nonalphabet
            for (char &ch : str) {
                if (isCapital(ch)) {
                    ch = tolower(ch);
                }
            }
            //upper to lower
        }
        Sentence tempsen;
        int sentnum = stoi(number);
        for (string &str : tempstr) {
            tempsen.srored[str] = sentnum;
        }
        tempsen.id = sentnum;
        AllSentence.push_back(tempsen);
    }
    
    //read query
    vector<string> keywords;
    ifstream queryFile(argv[2]);
    string query;
    unordered_map<string, int> stored_idf;
    while (getline(queryFile,query)) {
        keywords = tokenizer(query);
        for (auto &str : keywords) {
            for (char &ch : str) {
                if (isCapital(ch)) {
                    ch = tolower(ch);
                }
            }
        }
        
        //caculate keyword idf
        for (auto &str : keywords) {
            try {
                int idf = stored_idf.at(str);
            } catch (const out_of_range& e) {
                int appear_count = 0;
                for(auto &temp_sen : AllSentence) {
                    if (temp_sen.srored.find(str) != temp_sen.srored.end()) {
                        appear_count++;
                    }
                }
                stored_idf[str] = log((double)total_words/(double)appear_count);
            }
        }
        
        //store idf in allsen
        for(auto &temp_sent : AllSentence) {
            temp_sent.idf = 0;
            for (auto &str : keywords) {
                if (temp_sent.srored.find(str) != temp_sent.srored.end()) {
                    temp_sent.idf += stored_idf.at(str);
                }
            }
        }
        //find top k
        priority_queue<Sentence, vector<Sentence>, CompElement> topk;
        for (auto &tempsen : AllSentence) {
            if (topk.size() < k) {
                topk.push(tempsen);
            }
            else {
                if (topk.top().idf <= tempsen.idf) {
                    topk.push(tempsen);
                    topk.pop();
                }
            }
        }
        //print k
        vector<Sentence> result(k);
        for (int i = 0; i < k; i++) {
            result.push_back(topk.top());
            topk.pop();
        }
        for (int i = k-1; k >= 0; k--) {
            if (result[k].idf == 0) {
                printf("-1");
            }
            else {
                printf("%d", result[k].idf);
            }
            
            if (k != 0) {
                printf(" ");
            }
        }
        
    }
    return 0;
}
