#include "utils.hpp"

vector<vector<string>> routing_table;
vector<vector<int>> corr_machine;
vector<vector<pair<string,int>>> leaf_set;
int  b = 4, base = 16;

// Initialize routing table, leaf set and other data structures
void init_tables()
{
    routing_table.resize(log(n, base), vector<string>(base, ""));
    corr_machine.resize(log(n, base), vector<int>(base, -1));
    leaf_set.resize(2);
}

// distance function to calculate distance between two nodes
unsigned __int128 distance(string a, string b)
{
    vector<bool> byteA = convertHexToByte(a);
    vector<bool> byteB = convertHexToByte(b);
    unsigned __int128 x = 0, y = 0;
    for (int i = 0; i < byteA.size(); i++)
    {
        x = x * 2 + byteA[i];
    }
    for (int i = 0; i < byteB.size(); i++)
    {
        y = y * 2 + byteB[i];
    }
    if (x > y)
    {
        return x - y;
    }
    else
    {
        return y - x;
    }
}

void route(string key, string value, string sender)
{
    string hash = getHash(key);
    vector<bool> byte = convertHexToByte(hash);
    // Check if key can be routed to some node in the routing table
    int comPre = 0;
    for (int i = 0; i < nodeId.size(); i++)
    {
        if (nodeId[i] == hash[i])
        {
            comPre++;
        }
        else
        {
            break;
        }
    }
    bool found = false;
    if (comPre < log(n, base))
    {
        int row = comPre;
        int col;
        if (hash[comPre] >= '0' && hash[comPre] <= '9')
        {
            col = hash[comPre] - '0';
        }
        else
        {
            col = hash[comPre] - 'a' + 10;
        }
        if (routing_table[row][col] != "" && distance(routing_table[row][col], hash) < distance(nodeId, hash))
        {
            cout << "Using table Routing key: " << key << " value: " << value << " to node: " << corr_machine[row][col] << endl;
            string message = "store " + sender + " " + key + " " + value;
            send(corr_machine[row][col], message);
            found = true;
        }
    }
    if (!found)
    {
        // Check if key can be routed to some node in the leaf set, i.e., distance between hash and nodeId is less than distance between hash and current node
        unsigned __int128 minDist = distance(nodeId, hash);
        int closestNode = -1;
        for (int i = 0; i < leaf_set.size(); i++)
        {
            for (int j = 0; j < leaf_set[i].size(); j++)
            {
                unsigned __int128 dist = distance(leaf_set[i][j].first, hash);
                if (dist < minDist)
                {
                    minDist = dist;
                    closestNode = leaf_set[i][j].second;
                }
            }
        }
        if (closestNode != -1)
        {
            cout << "Using set Routing key: " << key << " value: " << value << " to node: " << closestNode << endl;
            string message = "store " + sender + " " + key + " " + value;
            send(closestNode, message);
            found = true;
        }
    }
    if (!found)
    {
        if(table.find(key) == table.end()) {
            table[key] = value;
            string message = "success " + to_string(id) + " " + key + " " + value;
            sendToApp(sender, message);
        }
        else {
            string message = "fail " + to_string(id) + " " + key;
            sendToApp(sender, message);
        }
    }
}

// Fetch the value of a key from the network
void fetch(string key, string sender)
{
    string hash = getHash(key);
    vector<bool> byte = convertHexToByte(hash);
    // Check if key can be routed to some node in the routing table
    int comPre = 0;
    for (int i = 0; i < nodeId.size(); i++)
    {
        if (nodeId[i] == hash[i])
        {
            comPre++;
        }
        else
        {
            break;
        }
    }
    bool found = false;
    if (comPre < log(n, base))
    {
        int row = comPre;
        int col;
        if (hash[comPre] >= '0' && hash[comPre] <= '9')
        {
            col = hash[comPre] - '0';
        }
        else
        {
            col = hash[comPre] - 'a' + 10;
        }
        if (routing_table[row][col] != "" && distance(routing_table[row][col], hash) < distance(nodeId, hash))
        {
            cout << "Using table Routing key: " << key << " to node: " << corr_machine[row][col] << endl;
            string message = "retrieve " + sender + " " + key;
            prt(distance(nodeId, hash));
            send(corr_machine[row][col], message);
            found = true;
        }
    }
    if (!found)
    {
        // Check if key can be routed to some node in the leaf set, i.e., distance between hash and nodeId is less than distance between hash and current node
        unsigned __int128 minDist = distance(nodeId, hash);
        int closestNode = -1;
        for (int i = 0; i < leaf_set.size(); i++)
        {
            for (int j = 0; j < leaf_set[i].size(); j++)
            {
                unsigned __int128 dist = distance(leaf_set[i][j].first, hash);
                if (dist < minDist)
                {
                    minDist = dist;
                    closestNode = leaf_set[i][j].second;
                }
            }
        }
        if (closestNode != -1)
        {
            cout << "Using set Routing key: " << key << " to node: " << closestNode << endl;
            string message = "retrieve " + sender + " " + key;
            send(closestNode, message);
            found = true;
        }
    }
    if (!found)
    {
        // Check for the key in the current node
        if (table.find(key) == table.end())
        {
            string message = "fail " + to_string(id) + " " + key;
            sendToApp(sender, message);
        }
        else
        {
            string message = "success " + to_string(id) + " " + key + " " + table[key];
            sendToApp(sender, message);
        }
    }
}

// Receive a join request from a new node
void join(string reqId, string sender)
{
    // add node to routing table if possible
    int comPre = 0;
    for (int i = 0; i < nodeId.size(); i++)
    {
        if (nodeId[i] == reqId[i])
        {
            comPre++;
        }
        else
        {
            break;
        }
    }
    if (comPre < log(n, base))
    {
        int row = comPre;
        int col;
        if (reqId[comPre] >= '0' && reqId[comPre] <= '9')
        {
            col = reqId[comPre] - '0';
        }
        else
        {
            col = reqId[comPre] - 'a' + 10;
        }
        routing_table[row][col] = reqId;
        corr_machine[row][col] = stoi(sender);
    }
    // add node to leaf set if possible
    if (reqId < nodeId)
    {
        leaf_set[0].push_back({reqId, stoi(sender)});
    }
    else
    {
        leaf_set[1].push_back({reqId, stoi(sender)});
    }
    sort(leaf_set[0].begin(), leaf_set[0].end());
    sort(leaf_set[1].begin(), leaf_set[1].end());
    if (leaf_set[0].size() > base)
    {
        leaf_set[0].pop_back();
    }
    if (leaf_set[1].size() > base)
    {
        leaf_set[1].erase(leaf_set[1].begin());
    }
}