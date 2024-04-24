#include "./include/kademlia.hpp"

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
            join(reqId, sender);
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

void doWork()
{
    while (1)
    {
        // cout << "\n\nChoose function" << endl;
        // cout << "1. Store Data" << endl;
        // cout << "2. Retrieve Data" << endl;
        // cout << "3. Exit" << endl;
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
        sleep(1);
        join();
        sleep(1);
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
        sleep(1);
        join();
        t.join();
    }

    for (int i = 0; i <= n; i++)
    {
        senders[i].close();
    }
    receiver.close();

    return 0;
}