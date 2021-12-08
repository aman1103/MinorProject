#include <bits/stdc++.h>

#include <ext/pb_ds/assoc_container.hpp>

using namespace std;

struct Node {
    int offset;
    int length;
    char next;

    Node(int of , int le, char ne) {
        offset = of ;
        length = le;
        next = ne;
    }
};

vector < Node * > Encode(string s) {
    vector < Node * > answer;

    const int buf_size = 3072;
    const int preview_size = 1024;

    int position = 0;

    while (position < (int) s.length()) {
        int max_index = 0, max_length = 0, cur_index, cur_length;
        for (int j = max(0, (position - buf_size)); j < position; j++) {
            cur_index = j, cur_length = 0;

            while (s[cur_index] == s[cur_length + position] && cur_index < min((position + preview_size), (int) s.length())) {
                cur_index++;
                cur_length++;
            }

            if (cur_length > max_length) {
                max_index = position - j;
                max_length = cur_length;
            }
        }

        position += max_length;
        answer.push_back(new Node(max_index, max_length, s[position]));
        position++;
    }

    return answer;
}

int main() {
    fstream fin;
    string input;
    char ch;
    fin.open("newfile.txt", ios:: in );
    if (fin.is_open()) {
        while (fin >> noskipws >> ch) {
        input+=ch;
        }
        cout<<input;
        fin.close();

        vector < Node * > encoded = Encode(input);

        fstream fout;
        fout.open("output.txt", ios::out);
        //fout << encoded.size() << " \n ";
        for (auto & node: encoded) {
            fout <<"<"<< node -> offset << "," << node -> length /*<< "," */<< ">"<< node -> next;
        }
        fout.close();
	}
	ifstream in_file("newfile.txt", ios::binary);
    in_file.seekg(0, ios::end);
    double s1 = in_file.tellg();
	ifstream in_file2("output.txt", ios::binary);
    in_file2.seekg(0, ios::end);
    double s2 = in_file2.tellg();
    double ratio = s1>s2 ? (s1-s2)/s1*100 : (s2-s1)/s1*100;
    cout<<s1<<" "<<s2<<endl;
    if(ratio > 100)
    {
		cout<<ratio<<" increased";
	}
	else
	{
		cout<<ratio<<" decreased";
	}
    return 0;
}
