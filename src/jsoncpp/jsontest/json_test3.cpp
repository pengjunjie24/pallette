/*************************************************************************
  > File Name: json_test3.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月03日 星期一 00时26分04秒
 ************************************************************************/
 
#include <iostream>
#include <string>
#include <fstream>

#include "../dist/json/json.h"

using namespace std;

void WriteJsonData(const char* filename)
{
    // 测试自定义json数据，存储在一个新的文件中
    {
        Json::Value root;

        // 写入到一个新的文件中，也可以当字符串使用
        std::ofstream os;
        os.open(filename, std::ios_base::out);

        root["retCode"] = "0000";
        root["retMsg"] = "成功";

        Json::Value arrayObj;   // 构建对象 
        arrayObj[0] = "000";
        arrayObj[1] = "001";
        arrayObj[2] = "002";
        root["adviceOpt"] = arrayObj;

        Json::Value arrayObj1;   // 构建对象
        arrayObj1[0]["keyBathNumber"] = "1";
        arrayObj1[0]["keyIdx"] = "01";
        arrayObj1[0]["keyValue"] = "1111111111";

        arrayObj1[1]["keyBathNumber"] = "2";
        arrayObj1[1]["keyIdx"] = "02";
        arrayObj1[1]["keyValue"] = "2222222222";
        root["keylist"] = arrayObj1;

        cout << root.toStyledString() << endl;
        os << root;
        os.close();
    }


    // 测试在一个有json数据的文件中读取数据然后添加或修改并存储在一个新的文件中
    {
    Json::Reader reader;
    Json::Value root;

    std::ifstream is;
    is.open(filename, std::ios::binary);
    if (reader.parse(is, root))
    {
        root["retMsg"] = "失败";

        root["adviceOpt"][2] = "003";
        root["adviceOpt"][3] = "002";

        root["keylist"][1]["keyIdx"] = "33";
        root["keylist"][1]["keyValue"] = "333333333";

        std::string out = root.toStyledString();
        cout << out << endl;

        // 输出无格式json字符串    
        Json::FastWriter writer;
        std::string strWrite = writer.write(root);
        std::ofstream ofs;
        ofs.open("test_write.json");
        ofs << strWrite;
        ofs.close();
    }
    is.close();
}
}

int main()
{
    // 读取的使用 
    // 1，读取字符串
    {
        /* {"encoding" : "UTF-8","plug-ins" : ["python","c++","ruby"],
        "indent" : { "length" : 3, "use_space": true}}*/
        string test_txt = "{\"encoding\" : \"UTF-8\",\"plug-ins\" : [\"python\",\"c++\",\
        \"ruby\"],\"indent\" : { \"length\" : 3, \"use_space\": true}}";
        cout << test_txt << endl;

        cout << "//////////////////////////////////////////////////////////////////////" << endl;
        Json::Reader reader;
        Json::Value value;
        if (reader.parse(test_txt, value))
        {
            // encoding
            cout << value["encoding"].asString() << endl;

            // 关于plug-ins
            // 方法1
            const Json::Value arrayObj1 = value["plug-ins"];
            for (int i = 0; i < static_cast<int>(arrayObj1.size()); ++i)
            {
                cout << arrayObj1[i].asString() << " ";
            }
            cout << endl;
            // 方法2
            cout << value["plug-ins"][0].asString() << " ";
            cout << value["plug-ins"][1].asString() << " ";
            cout << value["plug-ins"][2].asString() << " ";
            cout << endl;

            // 关于indent
            // 方法1
            const Json::Value arrayObj2 = value["indent"];
            cout << arrayObj2["length"].asString() << " ";
            cout << arrayObj2["use_space"].asString() << " ";
            cout << endl;
            // 方法2
            cout << value["indent"]["length"].asString() << " ";
            cout << value["indent"]["use_space"].asString() << " ";
            cout << endl;
        }
    }

    // 2，读取文件
    {
    Json::Value value;
    Json::Reader reader;
    std::ifstream ifs("example.json");//open file example.json

    if (reader.parse(ifs, value)){
        // success
        cout << value["encoding"].asString() << endl;
        cout << value["plug-ins"][0].asString() << endl;
        cout << value["indent"]["length"].asInt() << endl;
    }
    else{
        // fail to parse

    }
}

    // 3，写入本地文件
    {
        Json::Value value;
        Json::Reader reader;
        Json::FastWriter fwriter;
        Json::StyledWriter swriter;

        std::ifstream ifs("example.json");//open file example.json
        if (!reader.parse(ifs, value)){
            // parse fail
            return 0;
        }

        // 紧凑型
        std::string str = fwriter.write(value);
        std::ofstream ofs("example_fast_writer.json");
        ofs << str;
        ofs.close();

        // 排版型
        str = swriter.write(value);
        ofs.open("example_styled_writer.json");
        ofs << str;
        ofs.close();
    }


    {
        //1.从字符串解析json  
        const char* str = "{\"uploadid\": \"UP000000\",\"code\": 100,\
            \"msg\": \"\",\"files\": \"\"}";

        Json::Reader reader;
        Json::Value root;
        if (reader.parse(str, root))
        {
            printf("--------------从字符串读取JSON---------------\n");
            std::string upload_id = root["uploadid"].asString();  // upload_id = "UP000000"    
            int code = root["code"].asInt();                      // code = 100   

            printf("upload_id : %s\ncode : %d \n", upload_id.c_str(), code);
            printf("----------------------------------------------\n");
        }

        // 2. 向文件写入json 
        string sTempPath = "test_json.json";
        WriteJsonData(sTempPath.c_str());
    }

    return 0;
}
