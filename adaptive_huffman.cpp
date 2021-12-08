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

#define emptyString ""
double s2;
//Structure to creates nodes with the specific attributes such a ch and freq and left and right children
struct Node
{
    char ch;
    int freq;
    Node *left, *right;
};

//Utility function to create a node at any moment
Node* createNode(char ch, int freq, Node* left, Node* right)
{
	
    Node* node = new Node();
 
    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;
 
    return node;
}
// Checking if a node is leaf node or not
bool isLeaf(Node* root) {
    return root->left == nullptr && root->right == nullptr;
}

void encode(Node* root, string str, unordered_map<char, string> &huffmanCode)
{
	
}

void buildHuffmanTree(string text)
{
	
}
int main()
{	
	char ch;
	string text="";
	fstream fin("newfile.txt", fstream::in);
	//Collecting data from the text file
	while (fin >> noskipws >> ch) {
		if(ch!='\n')
		text+=ch;
	}
	//Closing the file for best practice and by not closing files we are wasting system resources.
	fin.close();
	//Calculating the logical size of the original file
	double s1 = text.length() * 8;
	double cr;
	//Get starting timepoint
    auto start = high_resolution_clock::now();
    //Function to build its huffman tree and encode it
    cout<<"The input text is: "<<text<<endl;
    buildHuffmanTree(text);
    //Get ending timepoint
    auto stop = high_resolution_clock::now();
    dbg(s1,s2);
    //Calculating the compression ratio
    cr = ((s1-s2)/s1)*100;
    //Calculating the time taken by the algorithm
    auto duration = duration_cast<microseconds>(stop - start);
    cout<<"Time taken by function: "<< duration.count()<<"microseconds" << endl;
    dbg(cr);
    return 0;
}
