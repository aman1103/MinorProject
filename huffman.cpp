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

//Custom sorting of the priority queue using comparator 
struct comp
{
    bool operator()(const Node* l, const Node* r) const
    {
        // the highest priority item has the lowest frequency
        return l->freq > r->freq;
    }
};

// Checking if a node is leaf node or not
bool isLeaf(Node* root) {
    return root->left == nullptr && root->right == nullptr;
}

void encode(Node* root, string str, unordered_map<char, string> &huffmanCode)
{
	//Return if root node is NULL aka the huffman tree is empty
    if(root == nullptr) {
        return;
    }
    if(isLeaf(root)) {
		//If its a leaf node add a '1' to its start if its the only one in the tree else str
        huffmanCode[root->ch] = (str != emptyString) ? str : "1";
    }
    //Going left adds a '0' to the encoded character
    encode(root->left, str + "0", huffmanCode);
    //Going right adds a '1' to the encoded character
    encode(root->right, str + "1", huffmanCode);
}

void buildHuffmanTree(string text)
{
	// If string is empty return
    if (text == emptyString) {
        return;
    }
    // store the occurances of each character and its frequency in an unordered map
    unordered_map<char, int> freq;
    for (char ch: text) {
        freq[ch]++;
    }
    // Make nodes of the character and its frequencies by popping from pq and add it to the huffman tree
    priority_queue<Node*, vector<Node*>, comp> pq;
    for (auto pair: freq) {
        pq.push(createNode(pair.first, pair.second, nullptr, nullptr));
    }
    //Run the code untill size of the priority queue is one meaning only the root node is left and its frequency is
    //the sum of all the characters in the file
    while (pq.size() != 1)
    {
		//Pop the top two nodes
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();
        //Add their frequencies and make a new node named '\0' which is null
        int sum = left->freq + right->freq;
        //Creating the '\0' node and making its children the top two popped frequencies
        pq.push(createNode('\0', sum, left, right));
    }
    // Only element is left in the pq which is the root node of the whole huffman tree
    Node* root = pq.top();
    // To show the codes we put the characters and their respective huffman code in an unordered map
    unordered_map<char, string> huffmanCode;
    //Encode the characters into their repsective huffman codes using the huffman tree created
    encode(root, emptyString, huffmanCode);
    //Displaying the huffman codes
    cout << "Huffman Codes are:\n" << endl;
    for (auto pair: huffmanCode) {
		//Printing the characters and their huffman codes
        cout << pair.first << " " << pair.second << endl;
    }
    string str;
    //Form a string of huffman codes to recreate the original text but in encoded form
    for (char ch: text) {
        str += huffmanCode[ch];
    }
    dbg(str);
    //Creating a file to dump the encoded string
    ofstream fout("compressed_huffman.txt");
    //You can stuff 8 bits into an unsigned char variable and write that to the file.
    //As putting the binary string as it is would nullify the effort of encoding it
	unsigned char ch=0;
	int i=0;
	while(str[i])
	{
		if(i%8 == 0) {
		fout << ch;
		ch = 0;
		}
		else {
		ch = ch | (str[i] - '0');
		ch = ch << 1;
		}
		i++;
	}
	//Calculating the logical size of the compressed file
	s2 = str.length();
	//Closing the file for best practice and by not closing files we are wasting system resources.
    fout.close();
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
/*Variables Used
 * emptyString = used in case the file is empty
 * fin = object of type fstream to input a stream of characters from a file
 * ch = takes character by character input from the file.
 * text = summation of all the characters in the file.
 * Node = the structure to create a tree.
 * freq = unordered map to store frequency of each character in the file.
 * pq = priority queue to store the frequency of each character. This is used in making to the tree
 * root = type node and the root of the huffman tree which is created using the priority queue
 * huffmanCode = map of key value pairs of the huffman tree
 * ch (type char of struct node) = store the null character if not a leaf node else the character of the node
 * freq (type int of struct node) = stores the frequency of the character occuring
 * left = pointer pointing to the left child
 * right = pointer pointing to the right child
 * 
*/

/*Functions Used
 * 1 - buildHuffmanTree
 * 
 * input = string
 * returns = void
 * use = Makes the huffman tree of characters and their frequencies as leaf nodes
 * 
 * 
 * 2 - createNode
 * 
 * input = char , int , Node* , Node*
 * returns = Node*
 * use = To create a node and add a pointer to the node whenever needed
 * 
 * 
 * 3 - encode
 * input = Node* , string , unordered_map
 * returns = void
 * use = To encode the characters in the huffmanTree and store them in <char,string> pairs in the huffmanCode
 * unordered map
 * 
 * 
 * 4 - isLeaf
 * input = Node*
 * returns = Boolean
 * use = To check
 * 
 * 
 * Comparator added to make priority queue in ascending order
 * 
 * 
 * */
