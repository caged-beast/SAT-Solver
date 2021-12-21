#include <iostream>
#include <fstream>
#include <string>
using namespace std;
#define N 4

int cnt = 0;//记录子句数量
void CreateNQueensFile(int n)//建立N皇后问题的CNF文件
{
    int i,j,k;
    string dataFileName = to_string(n)+"-Queen.txt";
    ofstream ofs(dataFileName,ios::out);
    // 每行至少有一个皇后
    for(i=1;i<=n;i++)
    {
        for(j=1;j<=n;j++)
        {
            ofs<<j+n*(i-1)<<" ";
        }
       ofs<<0<<endl;
       cnt++;
    }

    // 每行至多有一个皇后
    for(i=1;i<=n;i++)
    {
        for(j=1;j<=n;j++)
        {
            for(k=j+1;k<=n;k++)
            {
                ofs<<-(j+n*(i-1))<<" "<<-(k+n*(i-1))<<" 0"<<endl;
                cnt++;
            }
        }
    }

    // 每列至多有一个皇后
    for(j=1;j<=n;j++)
    {
        for(i=1;i<=n;i++)
        {
            for(k=i+1;k<=n;k++)
            {//列的间隔为4：1 5 9 13:4*(1-1)+1,4*(2-1)+1, ...
                ofs<<-(n*(i-1)+j)<<" "<<-(n*(k-1)+j)<<" 0"<<endl;
                cnt++;
            }
        }
    }

    //对角线至多有一个皇后
    for(i=1;i<=n;i++)
    {
        for(j=1;j<=n;j++)
        {
            for(k=1;k<=n;k++)
            {
                if(k!=i)
                {
                    if(j-i+k>=1 && j-i+k<=n)
                    {
                        ofs<<-(n*(i-1)+j)<<" "<<-(n*(k-1)+j-i+k)<<" 0"<<endl;
                        cnt++;
                    }
                    if(j-k+i>=1 && j-k+i<=n)
                    {
                        ofs<<-(n*(i-1)+j)<<" "<<-(n*(k-1)+j-k+i)<<" 0"<<endl;
                        cnt++;
                    }
                }
            }
        }
    }

    ofs.close();
    ofstream cnfFile(to_string(n)+"-Queen.cnf");
    ifstream data(dataFileName);
    cnfFile<<"p cnf "<<n*n<<" "<<cnt<<endl;//先生成前缀
    string buf;
    while(getline(data,buf))//再把子句信息逐行写入cnf文件
    {
        cnfFile<<buf<<endl;
    }
    cnfFile.close();
    data.close();
}

void Display(int n)//对求解器给出的结果进行可视化展示
{
    ifstream ifs(to_string(n)+"-out.txt");
    if(ifs.is_open())
    {
        string buf;
        getline(ifs,buf);//结果文件有两行，先读第一行
        if(buf=="SAT")
            cout<<"There is a solution:"<<endl<<endl;//有解准备解析显示
        else
        {
            cout<<"There is no solution"<<endl;//无解直接退出
            return;
        }
        getline(ifs,buf);//再读第二行
        int start = 0;
        int index = 0;//记录有多少个变元
        while(index<n*n)
        {
            int end = buf.find(" ",start);
            index++;
            int num = stoi(buf.substr(start,end));
            if(index==1)//每行之前加入空格，第一行要特殊处理
                cout<<"\t";
            if(num>0)
                cout<<"O ";
            else
                cout<<"+ ";
            if(num%n==0)//每行之前加入空格
                cout<<endl<<"\t";
            start = end+1;
        }
        cout<<endl<<endl;
    }
    else
        cout<<"文件打开失败"<<endl;
}

int main()
{
    // CreateNQueensFile(N);
    // Display(N);
    
    system("pause");
    return 0;
}
