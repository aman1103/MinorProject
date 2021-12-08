#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
using namespace std;
using namespace std::chrono;
using namespace __gnu_pbds;
void __print(int x) {cerr << x;}
void __print(long x) {cerr << x;}
void __print(long long x) {cerr << x;}
void __print(unsigned x) {cerr << x;}
void __print(unsigned long x) {cerr << x;}
void __print(unsigned long long x) {cerr << x;}
void __print(float x) {cerr << x;}
void __print(double x) {cerr << x;}
void __print(long double x) {cerr << x;}
void __print(char x) {cerr << '\'' << x << '\'';}
void __print(const char *x) {cerr << '\"' << x << '\"';}
void __print(const string &x) {cerr << '\"' << x << '\"';}
void __print(bool x) {cerr << (x ? "true" : "false");}

template<typename T, typename V>
void __print(const pair<T, V> &x) {cerr << '{'; __print(x.first); cerr << ','; __print(x.second); cerr << '}';}
template<typename T>
void __print(const T &x) {int f = 0; cerr << '{'; for (auto &i: x) cerr << (f++ ? "," : ""), __print(i); cerr << "}";}
void _print() {cerr << "]\n";}
template <typename T, typename... V>
void _print(T t, V... v) {__print(t); if (sizeof...(v)) cerr << ", "; _print(v...);}
#ifndef ONLINE_JUDGE
#define dbg(x...) cerr <<"[" << #x << "] = ["; _print(x)
#else
#define dbg(x...)
#endif

#define rep(i, n) rep2(i, 0, n)
#define rep2(i, m, n) for (int i = m; i < (n); i++)
#define per(i, b) per2(i, 0, b)
#define per2(i, a, b) for (int i = int(b) - 1; i >= int(a); i--)
#define IOS ios::sync_with_stdio(0); cin.tie(0); cout.tie(0);
#define endl "\n"
#define ll long long
typedef vector<int> vi;
typedef vector< vi > vvi;
typedef pair< int,int > ii;
typedef pair< double,double> dd;
typedef tree<int, null_type,less<int>,rb_tree_tag,tree_order_statistics_node_update> pbds;
#define pb push_back
#define eb emplace_back
#define sz(a) (int)a.size()
#define all(c) c.begin(),c.end()
#define mp make_pair
#define fi first
#define se second
#define pres(c, val) (find((c).begin(), (c).end(), val) != (c).end())
const int MOD = 1000000007;
const int INF = INT_MAX;
const long long INFLL = (ll)INF * (ll)INF;

//New string variable for compressed string
string str2="";

void encode(string str)
{
	dbg(str);
	//Calculating the size of the input string
    int n = str.length();
    for (int i = 0; i < n; i++) {
		//Initially if the character is present in the string it is present one time
        int count = 1;
        while (i < n - 1 && str[i] == str[i + 1])
        {
            //Count increses if there are continuous occurances of the same character
            count++;
            //Iterating to the next character
            i++;
        }
        //dbg(i,n);
        //If null character then go back
        if(i == n-1)
        {
			break;
		}
		//Add the character once to the compressed string
        str2+=str[i];
        //Convering the frequency to string to concatinate better
        string number = to_string(count);
        //dbg(number);
        //Add the frequency to the compressed string
		str2+=number;
        //dbg(str2);
    }
    dbg(str2);
    //Creating an object of ofstream to write into the file the compressed string
    ofstream fout("compressed_RLE.txt");
    int i=0;
	//While the string is not finished
	while(str2[i])
	{
		//Adding character to the file
		fout << str2[i];
		i++;
	}
	//Closing the file for best practice and by not closing files we are wasting system resources.
    fout.close();
}		
int main(){
	
    char ch;
    string text="";
    //Creating object of fstream to read a file 
    fstream fin("newfile.txt", fstream::in);
    while (fin >> noskipws >> ch) {
        text+=ch;
    }
    //Closing the file for best practice and by not closing files we are wasting system resources.
    fin.close();
    //Calculating logical size of the original file
    double s1 = (text.length()-1) * 8;
    //Compression ratio variable
    double cr;
    //Get starting timepoint
    auto start = high_resolution_clock::now();
    encode(text);
    //Get ending timepoint
    auto stop = high_resolution_clock::now();
    double s2 = str2.length()*8;
    cout<<"\n";
    cout<<"Original size: "<<s1<<" "<<"Compresses size: "<<s2<<"\n";
    cr = ((s1-s2)/s1);
    if(cr <= 0)
    {
		cout<<"File not suited for compression using RLE"<<endl;
	}
    cout<<"Compression ratio of the algorithm: "<<cr<<endl;
    //Calculating the time taken by the algorithm
    auto duration=duration_cast<microseconds>(stop-start);
    cout<<"Time taken by algorithm: "<<duration.count()<<"microseconds" << endl;
}

/* Variables used
 * ch = takes character by character input from the file.
 * text = summation of all the characters in the file.
 * n = length of the whole text string.
 * count = used to keep track of continuous occurances of a character.
 * str = text
 * number = string converted
 * str2 = the compressed string
 * 
 * 
 * Functions Used
 * 1 - encode
 * input = string
 * returns = void
 * use = encodes the string into RLE encoded string
 * 
 * 
 * 
 * 
 * */
