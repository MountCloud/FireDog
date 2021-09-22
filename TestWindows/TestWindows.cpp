// TestWindows.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include "map/avlmap.h"
#include "map/hashmap.h"
#include "stringutil.h"
#include "matcher.h"
using namespace std;
using namespace firedog;


void test() {
    AvlMap<char, MatcherByteData*>* map = new AvlMap<char, MatcherByteData*>();
    MatcherByteData* data = new MatcherByteData();
    data->feature = new MatcherFeature();
    data->feature->author = "123";
    char c = 't';
    map->insert(c, data);

    AvlMap<char, MatcherByteData*>::Iterator iter = map->find(c);
    AvlMap<char, MatcherByteData*>::Iterator iter1 = map->find('a');

    int iii = 0;
}

void test1() {
    MatcherFeature* mf = new MatcherFeature();
    mf->name = "12123";
    mf->author = "213";
    mf->describe = "1231";

    HashMap<string, MatcherFeature*>* hashmap = new HashMap<string, MatcherFeature*>(16,"");
    hashmap->emplace("123", mf);

    MatcherFeature* tmf = hashmap->at("123");

    try {
        MatcherFeature* tmf1 = hashmap->at("1231");
    }
    catch (std::out_of_range ex) {
        cout << "notfound" << endl;
    }

    int iii = 0;
}

int main()
{
    std::cout << "Hello World!\n";

    test();
    test1();

    AvlMap<int, string> map;

    map.insert(1, "123123");
    map.insert(2, "5666");
    map.insert(123, "65156123321");
    map.insert(23, "521231");
    AvlMap<int, string>::Iterator findi =  map.find(3);
    string temp = map[123];
    
    std::cout << temp << endl;


    HashMap<string, string> hashmap(16,"");
    hashmap["test"] = "test";
    hashmap["hello"] = "hello";
    hashmap["world"] = "world";

    string tv = hashmap["test"];
    string abc = hashmap["abc"];

    string sss = StringUtil::trim(" 12312 ");

    cout << sss << endl;


    string teststr = "test我";
    string hexstr = StringUtil::textToHexText(teststr);

    vector<char> bytes;
    StringUtil::hexTextToBytes(hexstr,&bytes);

    int iii = 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
