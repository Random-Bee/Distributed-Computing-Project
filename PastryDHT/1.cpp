// Pastry DHT

#include <bits/stdc++.h>
#include <sys/wait.h>
#include <zmq.hpp>
#include <openssl/evp.h>
#include <semaphore.h>
#include <sys/time.h>
using namespace std;

const string base_endpoint = "tcp://127.0.0.1:";

int n, b = 4, base = 16, id;
string nodeId;

unordered_map<string, string> table;

// List of sockets for sending messages to other processes
zmq::socket_t* senders;

// Socket for receiving messages from other processes
zmq::socket_t receiver;

// Context for zmq
zmq::context_t context;

// Initialize the sockets and process variables
void init() {
    // initialize sockets
    senders = new zmq::socket_t[n+1];
    context = zmq::context_t(1);
    receiver = zmq::socket_t(context, ZMQ_PULL);
    receiver.bind(base_endpoint + to_string(5555 + id));
    // 0 to n-1 processes are workers
    // n-th process is parent process, it will send completion message to all processes
    for(int i=0; i<=n; i++) {
        senders[i] = zmq::socket_t(context, ZMQ_PUSH);
        senders[i].connect(base_endpoint + to_string(5555 + i));
    }
}

// log function to calculate log base y of x
int log(int x, int y) {
    int ans = 0, c = 1;
    while(c < x) {
        c *= y;
        ans++;
    }
    return ans;
}

vector<bool> convertHexToByte(string hex) {
    vector<bool> byte;
    for(int i = 0; i < hex.size(); i++) {
        if(hex[i] >= '0' && hex[i] <= '9') {
            int val = hex[i] - '0';
            for(int j = 0; j < 4; j++) {
                byte.push_back(val % 2);
                val /= 2;
            }
        } else {
            int val = hex[i] - 'a' + 10;
            for(int j = 0; j < 4; j++) {
                byte.push_back(val % 2);
                val /= 2;
            }
        }
    }
    return byte;
}

string convertByteToHex(vector<bool> byte) {
    string hex = "";
    for(int i = 0; i < byte.size(); i += 4) {
        int val = 0;
        for(int j = 0; j < 4; j++) {
            val += byte[i + j] * pow(2, j);
        }
        if(val < 10) {
            hex += (char)(val + '0');
        } else {
            hex += (char)(val - 10 + 'a');
        }
    }
    return hex;
}

string getHash(const string& str) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, str.c_str(), str.length());
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digestLength;
    EVP_DigestFinal_ex(ctx, digest, &digestLength);
    EVP_MD_CTX_free(ctx);
    char sha256String[digestLength+1];
    for(int i = 0; i < digestLength/2; i++) {
        sprintf(&sha256String[i*2], "%02x", (unsigned int)digest[i]);
    }
    return string(sha256String);
}

// distance function to calculate distance between two nodes
unsigned __int128 distance(string a, string b) {
    vector<bool> byteA = convertHexToByte(a);
    vector<bool> byteB = convertHexToByte(b);
    unsigned __int128 x = 0, y = 0;
    for(int i = 0; i < byteA.size(); i++) {
        x = x * 2 + byteA[i];
    }
    for(int i = 0; i < byteB.size(); i++) {
        y = y * 2 + byteB[i];
    }
    if (x > y) {
        return x - y;
    } else {
        return y - x;
    }
}

void generateNodeId() {
    nodeId = getHash(base_endpoint + to_string(id));
}

vector<vector<string>> routing_table;
vector<vector<int>> corr_machine;
vector<vector<pair<string,int>>> leaf_set;

void send(int dest, string message) {
    zmq::message_t msg(message.size());
    memcpy(msg.data(), message.c_str(), message.size());
    senders[dest].send(msg, zmq::send_flags::none);
}

void route(string key, string value, int sender) {
    string hash = getHash(key);
    vector<bool> byte = convertHexToByte(hash);
    // Check if key can be routed to some node in the routing table
    int comPre = 0;
    for(int i = 0; i < nodeId.size(); i++) {
        if(nodeId[i] == hash[i]) {
            comPre++;
        } else {
            break;
        }
    }
    bool found = false;
    if(comPre < log(n, base)) {
        int row = comPre;
        int col;
        if(hash[comPre] >= '0' && hash[comPre] <= '9') {
            col = hash[comPre] - '0';
        } else {
            col = hash[comPre] - 'a' + 10;
        }
        if(routing_table[row][col] != "" && distance(routing_table[row][col], hash) < distance(nodeId, hash)) {
            cout << "Using table Routing key: " << key << " value: " << value << " to node: " << corr_machine[row][col] << endl;
            string message = "store " + to_string(sender) + " " + key + " " + value;
            send(corr_machine[row][col], message);
            found = true;
        }
    }
    if(!found) {
        // Check if key can be routed to some node in the leaf set, i.e., distance between hash and nodeId is less than distance between hash and current node
        unsigned __int128 minDist = distance(nodeId, hash);
        int closestNode = -1;
        for(int i=0; i<leaf_set.size(); i++) {
            for(int j=0; j<leaf_set[i].size(); j++) {
                unsigned __int128 dist = distance(leaf_set[i][j].first, hash);
                if(dist < minDist) {
                    minDist = dist;
                    closestNode = leaf_set[i][j].second;
                }
            }
        }
        if(closestNode != -1) {
            cout << "Using set Routing key: " << key << " value: " << value << " to node: " << closestNode << endl;
            string message = "store " + to_string(sender) + " " + key + " " + value;
            send(closestNode, message);
            found = true;
        }
    }
    if(!found) {
        // Store the key in the current node
        table[key] = value;
        string message = "log " + to_string(id) + " " + key + " " + value;
        send(sender, message);
    }
}

void prt(unsigned __int128 x) {
    string s = "";
    while(x > 0) {
        s += (char)(x % 10 + '0');
        x /= 10;
    }
    reverse(s.begin(), s.end());
    cout << s << endl;
}

void fetch(string key, int sender) {
    string hash = getHash(key);
    vector<bool> byte = convertHexToByte(hash);
    // Check if key can be routed to some node in the routing table
    int comPre = 0;
    for(int i = 0; i < nodeId.size(); i++) {
        if(nodeId[i] == hash[i]) {
            comPre++;
        } else {
            break;
        }
    }
    bool found = false;
    if(comPre < log(n, base)) {
        int row = comPre;
        int col;
        if(hash[comPre] >= '0' && hash[comPre] <= '9') {
            col = hash[comPre] - '0';
        } else {
            col = hash[comPre] - 'a' + 10;
        }
        if(routing_table[row][col] != "" && distance(routing_table[row][col], hash) < distance(nodeId, hash)) {
            cout << "Using table Routing key: " << key << " to node: " << corr_machine[row][col] << endl;
            string message = "retrieve " + to_string(id) + " " + key;
            prt(distance(nodeId, hash));
            send(corr_machine[row][col], message);
            found = true;
        }
    }
    if(!found) {
        // Check if key can be routed to some node in the leaf set, i.e., distance between hash and nodeId is less than distance between hash and current node
        unsigned __int128 minDist = distance(nodeId, hash);
        cout << "MinDist: ";
        prt(minDist);
        int closestNode = -1;
        for(int i=0; i<leaf_set.size(); i++) {
            for(int j=0; j<leaf_set[i].size(); j++) {
                unsigned __int128 dist = distance(leaf_set[i][j].first, hash);
                if(dist < minDist) {
                    minDist = dist;
                    closestNode = leaf_set[i][j].second;
                }
            }
        }
        if(closestNode != -1) {
            cout << "Using set Routing key: " << key << " to node: " << closestNode << endl;
            string message = "retrieve " + to_string(id) + " " + key;
            prt(minDist);
            send(closestNode, message);
            found = true;
        }
    }
    if(!found) {
        // Check for the key in the current node
        for(auto x: table) {
            cout << x.first << " " << x.second << endl;
        }
        if(table.find(key) == table.end()) {
            string message = "fail " + to_string(id) + " " + key;
            send(sender, message);
        }
        else {
            string message = "success " + to_string(id) + " " + key + " " + table[key];
            send(sender, message);
        }
    }
}

set<int> rem_nodes;
int joined = 0;
int ready = 0;

void receive() {
    while(1) {
        zmq::message_t msg;
        zmq::recv_result_t res = receiver.recv(msg, zmq::recv_flags::none);
        string message(static_cast<char*>(msg.data()), msg.size());
        stringstream ss(message);
        string type, sender;
        ss >> type >> sender;
        if(type == "join") {
            string reqId;
            ss >> reqId;
            if(rem_nodes.find(stoi(sender)) == rem_nodes.end()) {
                continue;
            }
            // add node to routing table if possible
            int comPre = 0;
            for(int i = 0; i < nodeId.size(); i++) {
                if(nodeId[i] == reqId[i]) {
                    comPre++;
                } else {
                    break;
                }
            }
            if(comPre < log(n, base)) {
                int row = comPre;
                int col;
                if(reqId[comPre] >= '0' && reqId[comPre] <= '9') {
                    col = reqId[comPre] - '0';
                } else {
                    col = reqId[comPre] - 'a' + 10;
                }
                routing_table[row][col] = reqId;
                corr_machine[row][col] = stoi(sender);
            }
            // add node to leaf set if possible
            if(reqId < nodeId) {
                leaf_set[0].push_back({reqId, stoi(sender)});
            } else {
                leaf_set[1].push_back({reqId, stoi(sender)});
            }
            sort(leaf_set[0].begin(), leaf_set[0].end());
            sort(leaf_set[1].begin(), leaf_set[1].end());
            if(leaf_set[0].size() > base) {
                leaf_set[0].pop_back();
            }
            if(leaf_set[1].size() > base) {
                leaf_set[1].erase(leaf_set[1].begin());
            }
            // send ack to the sender
            string message = "ack " + to_string(id);
            send(stoi(sender), message);
            rem_nodes.erase(stoi(sender));
        }
        else if(type == "ack") {
            joined++;
        }
        else if(type == "ready") {
            ready++;
        }
        else if(type == "store") {
            string key, value;
            ss >> key >> value;
            route(key, value, stoi(sender));
        }
        else if(type == "retrieve") {
            string key;
            ss >> key;
            fetch(key, stoi(sender));
        }
        else if(type == "fail") {
            string key;
            ss >> key;
            cout << "Key: " << key << " not found in " << sender << endl;
        }
        else if(type == "success") {
            string key, value;
            ss >> key >> value;
            cout << "Key: " << key << " value: " << value << " found in node: " << sender << endl;
        }
        else if(type == "log") {
            string key, value;
            ss >> key >> value;
            table[key] = value;
            cout << "Stored key: " << key << " value: " << value << " in node: " << sender << endl;
        }
        else if(type == "exit") {
            break;
        }
    }
}

void init_tables() {
    routing_table.resize(log(n, base), vector<string>(base, ""));
    corr_machine.resize(log(n, base), vector<int>(base, -1));
    leaf_set.resize(2);
}

void join() {
    string message = "join " + to_string(id) + " " + nodeId;
    for(int i=0; i<n; i++) {
        if(i == id) continue;
        send(i, message);
    }
}

void wait_for_all() {
    for(int i=0; i<n; i++) {
        if(i == id) continue;
        rem_nodes.insert(i);
    }
    while(joined < n-1) {
        join();
        // wait before sending join request again
        sleep(1);
    }
    string message = "ready " + to_string(n-1);
    send(n-1, message);
}


void doWork() {
    cout << "Choose function" << endl;
    cout << "1. Store Data" << endl;
    cout << "2. Retrieve Data" << endl;
    cout << "3. Exit" << endl;
    while(1) {
        int choice;
        cin >> choice;

        if(choice==1) {
            string key, value;
            cout << "Enter key: ";
            cin >> key;
            cout << "Enter value: ";
            cin >> value;
            route(key, value, id);
        }
        else if(choice==2) {
            string key;
            cout << "Enter key: ";
            cin >> key;
            fetch(key, id);
        }
        else if(choice==3) {
            string message = "exit " + to_string(id);
            for(int i=0; i<n; i++) {
                send(i, message);
            }
            break;
        }
        else {
            cout << "Invalid choice" << endl;
        }
    }
}

int main() {
    int i;

    FILE* file = fopen("inp.txt", "r");

    if(file == NULL) {
        cout << "Error in opening the file" << endl;
        return 0;
    }

    fscanf(file, "%d", &n);

    fclose(file);

    for(i = 0; i < n-1; i++) {
        if(fork() == 0) {
            break;
        }
    }

    id = i;

    if(i==n-1) {
        init();
        generateNodeId();
        init_tables();
        thread t(receive);
        wait_for_all();
        while(ready < n);
        thread t1(doWork);
        t.join();
        t1.join();
        for(i=0; i<n-1; i++) {
            wait(NULL);
        }
    }
    else {
        init();
        generateNodeId();
        init_tables();
        thread t(receive);
        wait_for_all();
        t.join();
    }

    for(int i=0; i<=n; i++) {
        senders[i].close();
    }
    receiver.close();

    return 0;
}