/*
    This is a part of Tocy's Sample Code
    demo to use JsonCpp 
    https://github.com/open-source-parsers/jsoncpp
*/

#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include "../dist/json/json.h"

using namespace std;

// =========================== parse-json ==================================
void demo_simple()
{
    ifstream ifs;
    ifs.open("checkjson.json");
    /*
        In this file, its content is : (key-value)
        {
        "name" : "tocy",
        "age" : 1000
        }
    */
    assert(ifs.is_open());
 
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(ifs, root, false))
    {
        cerr << "parse failed \n";
        return;
    }
 
    string name = root["name"].asString();
    int age = root["age"].asInt();
 
    cout << "demo read from filet result =============\n";
    cout << "name : " << name << endl;
    cout << "age : " << age << endl;
    cout << endl;
    
    ifs.close();
}

void demo_parse_mem_object()
{
    const char json_data[] = 
        "{\"name\" : \"Tocy\", \"salary\" : 100, \"msg\" : \"work hard\", \
        \"files\" : [\"1.ts\", \"2.txt\"]}";  

    Json::Reader reader;
    Json::Value root;
    // reader将Json字符串解析到root，root将包含Json里所有子元素  
    if (!reader.parse(json_data, json_data + sizeof(json_data), root))
    {
        cerr << "json parse failed\n";
        return;
    }
    
    cout << "demo read from memory using object ---------\n";
    string name = root["name"].asString();
    int salary = root["salary"].asInt();
    string msg = root["msg"].asString();
    cout << "name: " << name << " salary: " << salary;
    cout << " msg: " << msg << endl;
    cout << "enter files: \n";
    Json::Value files = root["files"]; // read array here
    for (unsigned int i = 0; i < files.size(); ++i)
    {
        cout << files[i].asString() << " ";
    }
    cout << endl << endl;
}

void demo_parse_mem_array()
{
    const char json_data[] = 
        "[{\"name\" : \"Tocy\", \"salary\" : 100}, {\"name\" : \"Kit\", \"salary\" : 89}, \
        \"a json note\"]";  

    Json::Reader reader;
    Json::Value root;
    // reader将Json字符串解析到root，root将包含Json里所有子元素  
    if (!reader.parse(json_data, json_data + sizeof(json_data), root))
    {
        cerr << "json parse failed\n";
        return;
    }
    
    cout << "demo read from memory using array---------\n";
    unsigned int count = root.size() - 1;
    for (unsigned int i = 0; i < count; ++i)
    {
        string name = root[i]["name"].asString();
        int salary = root[i]["salary"].asInt();
        cout << "name: " << name << " salary: " << salary << endl;
    }
    cout << "last msg: " << root[count].asString() << endl;
    cout << endl << endl;
}


// =================== save-json =====================================
void demo_write_simple()
{
    Json::Value root;
    Json::FastWriter writer;
    Json::Value person;
 
    person["name"] = "tocy";
    person["age"] = 1000;
    root.append(person);
 
    string json_file = writer.write(root);
    cout << "demo write json ==============\n";
    cout << json_file << endl;
}

void demo_write_object()
{
    Json::Value root;
    Json::FastWriter writer;
 
    root["name"] = "tocy";
    root["salary"] = 100;
    root["msg"] = "work hard";
    Json::Value files;
    files[0] = "1.ts";
    files[1] = "2.txt";
    root["files"] = files;
 
    string json_file = writer.write(root);
    cout << "demo write json object ==============\n";
    cout << json_file << endl;
}

void demo_write_array()
{
    Json::Value root;
    Json::FastWriter writer;

    {
        Json::Value person; 
        person["name"] = "Tocy";
        person["salary"] = 100;
        root[0] = person;
    }
    
    {
        Json::Value person; 
        person["name"] = "Kit";
        person["salary"] = 89;
        root[1] = person;
    }
    
    root[2] = "a json note";   
 
    string json_file = writer.write(root);
    cout << "demo write json ==============\n";
    cout << json_file << endl;
}

int main()
{
    demo_simple();
    demo_parse_mem_object();
    demo_parse_mem_array();
    
    cout << "\n\nnext are write json example\n================================\n";

    demo_write_simple();
    demo_write_object();
    demo_write_array();
    return 0;
}
