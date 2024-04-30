
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <zmq.hpp>
#include <openssl/evp.h>
#include <semaphore.h>
#include <sys/time.h>
#include "time.hpp"
using namespace std;

#define LOG_FILE "log.txt"
#define TIME_FILE "time.txt"
#define KEY_FILE "key.txt"
ofstream logFile = ofstream(LOG_FILE, ios::app);

int key_resp = 0;

void printLogFile(int hops)
{
    string s = to_string(hops) + "\n";
    logFile << s;
}

void printkeyFile()
{
    string s = to_string(key_resp) + "\n";
    ofstream keyFile = ofstream(KEY_FILE, ios::app);
    keyFile << s;
}

void printTimeFile(string start, string end){
    long long diff = diffTime(start, end);
    string s = to_string(diff) + "\n";
    ofstream timeFile = ofstream(TIME_FILE, ios::app);
    timeFile << s;
}

// Base endpoint for all the processes
vector<string> base_endpoint = {};

// Size of machine Numbers
int num_machine;

string getAddress(int id)
{
    int machineNo = id % num_machine;
    int portNo = 5555 + id / num_machine;
    string address = base_endpoint[machineNo] + ":" + to_string(portNo);
    return address;
}

// Number of processes and id of the current process
int n, id;

// Node id of the current process: sha256(base_endpoint + id)
string nodeId;

// Table that stores key value pairs
unordered_map<string, string> table;

// List of sockets for sending messages to other processes
zmq::socket_t *senders;

// Socket for receiving messages from other processes
zmq::socket_t receiver;

// Context for zmq
zmq::context_t context;

// Initialize the sockets and process variables
void init()
{
    // initialize sockets
    senders = new zmq::socket_t[n + 1];
    context = zmq::context_t(1);
    receiver = zmq::socket_t(context, ZMQ_PULL);
    receiver.bind(getAddress(id));
    // 0 to n-1 processes are workers
    // n-th process is parent process, it will send completion message to all processes
    for (int i = 0; i <= n; i++)
    {
        senders[i] = zmq::socket_t(context, ZMQ_PUSH);
        senders[i].connect(getAddress(i));
    }
}

// log function to calculate log base y of x
int log(int x, int y)
{
    int ans = 0, c = 1;
    while (c < x)
    {
        c *= y;
        ans++;
    }
    return ans;
}

// Convert hex string to byte array
vector<bool> convertHexToByte(string hex)
{
    vector<bool> byte;
    for (int i = 0; i < hex.size(); i++)
    {
        if (hex[i] >= '0' && hex[i] <= '9')
        {
            int val = hex[i] - '0';
            for (int j = 0; j < 4; j++)
            {
                byte.push_back(val % 2);
                val /= 2;
            }
        }
        else
        {
            int val = hex[i] - 'a' + 10;
            for (int j = 0; j < 4; j++)
            {
                byte.push_back(val % 2);
                val /= 2;
            }
        }
    }
    return byte;
}

// Convert byte array to hex string
string convertByteToHex(vector<bool> byte)
{
    string hex = "";
    for (int i = 0; i < byte.size(); i += 4)
    {
        int val = 0;
        for (int j = 0; j < 4; j++)
        {
            val += byte[i + j] * pow(2, j);
        }
        if (val < 10)
        {
            hex += (char)(val + '0');
        }
        else
        {
            hex += (char)(val - 10 + 'a');
        }
    }
    return hex;
}

// Get sha256 hash of a string
string getHash(const string &str)
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, str.c_str(), str.length());
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digestLength;
    EVP_DigestFinal_ex(ctx, digest, &digestLength);
    EVP_MD_CTX_free(ctx);
    char sha256String[digestLength + 1];
    for (int i = 0; i < digestLength / 2; i++)
    {
        sprintf(&sha256String[i * 2], "%02x", (unsigned int)digest[i]);
    }
    return string(sha256String);
}

// Generate node id of the current process
void generateNodeId()
{
    nodeId = getHash(getAddress(id));
}

// Send message to a process
void send(int dest, string message)
{
    zmq::message_t msg(message.size());
    memcpy(msg.data(), message.c_str(), message.size());
    senders[dest].send(msg, zmq::send_flags::none);
}

// Print unsigned __int128
void prt(unsigned __int128 x)
{
    string s = "";
    while (x > 0)
    {
        s += (char)(x % 10 + '0');
        x /= 10;
    }
    reverse(s.begin(), s.end());
    cout << s << endl;
}

// Sending a join message to all other processes to join the network
void join()
{
    string message = "join " + to_string(id) + " " + nodeId;
    for (int i = 0; i < n; i++)
    {
        if (i == id)
            continue;
        send(i, message);
    }
}

void printTable()
{
    for (auto it : table)
    {
        cout << it.first << " " << it.second << endl;
    }
}

string messageCreator(string command, int id = -1, string key = "", int hop = -1, string value = "")
{
    string s = command;
    if (id == -1)
        return s;
    s += " " + to_string(id);
    if (key == "")
        return s;
    s += " " + key;
    if (hop == -1)
        return s;
    s += " " + to_string(hop);
    if (value == "")
        return s;
    s += " " + value;
    return s;
}
