#include "./include/kademlia.hpp"

int joined = 0;
int ready = 0;
set<int> rem_nodes;

void receive()
{
    while (1)
    {
        zmq::message_t msg;
        zmq::recv_result_t res = receiver.recv(msg, zmq::recv_flags::none);
        string message(static_cast<char *>(msg.data()), msg.size());
        stringstream ss(message);
        string type, sender;
        ss >> type >> sender;
        if (type == "join")
        {
            string reqId;
            ss >> reqId;
            if(rem_nodes.find(stoi(sender)) == rem_nodes.end()) {
                continue;
            }
            join(reqId, sender);
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
        else if (type == "store")
        {
            string key, value;
            ss >> key >> value;
            route(key, value, stoi(sender));
        }
        else if (type == "retrieve")
        {
            string key;
            ss >> key;
            fetch(key, stoi(sender));
        }
        else if (type == "fail")
        {
            string key;
            ss >> key;
            cout << "Key: " << key << " not found in " << sender << endl;
        }
        else if (type == "success")
        {
            string key, value;
            ss >> key >> value;
            cout << "Key: " << key << " value: " << value << " found in node: " << sender << endl;
        }
        else if (type == "log")
        {
            string key, value;
            ss >> key >> value;
            table[key] = value;
            cout << "Stored key: " << key << " value: " << value << " in node: " << sender << endl;
        }
        else if (type == "exit")
        {
            break;
        }
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

void doWork()
{
    cout << "\n\nChoose function" << endl;
    cout << "1. Store Data" << endl;
    cout << "2. Retrieve Data" << endl;
    cout << "3. Exit" << endl;
    while (1)
    {
        int choice;
        cin >> choice;

        if (choice == 1)
        {
            string key, value;
            cout << "Enter key: ";
            cin >> key;
            cout << "Enter value: ";
            cin >> value;
            route(key, value, id);
        }
        else if (choice == 2)
        {
            string key;
            cout << "Enter key: ";
            cin >> key;
            fetch(key, id);
        }
        else if (choice == 3)
        {
            string message = "exit " + to_string(id);
            for (int i = 0; i < n; i++)
            {
                send(i, message);
            }
            break;
        }
        else if (choice == 4)
        {
            printRoutingTable();
        }
        else if (choice == 5)
        {
            printCorrMachine();
        }
        else
        {
            cout << "Invalid choice" << endl;
        }
    }
}

int main()
{
    int i;

    FILE *file = fopen("inp.txt", "r");

    if (file == NULL)
    {
        cout << "Error in opening the file" << endl;
        return 0;
    }

    fscanf(file, "%d", &n);

    fclose(file);

    for (i = 0; i < n - 1; i++)
    {
        if (fork() == 0)
        {
            break;
        }
    }

    id = i;

    if (i == n - 1)
    {
        init();
        generateNodeId();
        init_tables();
        thread t(receive);
        wait_for_all();
        while(ready < n);
        thread t1(doWork);
        t.join();
        t1.join();
        for (i = 0; i < n - 1; i++)
        {
            wait(NULL);
        }
    }
    else
    {
        init();
        generateNodeId();
        init_tables();
        thread t(receive);
        wait_for_all();
        t.join();
    }

    for (int i = 0; i <= n; i++)
    {
        senders[i].close();
    }
    receiver.close();

    return 0;
}