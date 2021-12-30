#include <iostream>
#include <fstream>
#include <string>
using namespace std;
#define N 32

//一个链表存放一个子句，每个节点存放一个变量，头节点存放有变量个数
typedef struct Node//数据节点
{
    int val;//变量的值
    struct Node *next;
}Node;

typedef struct//用一个邻接表来存放CNF文件数据
{
    Node ** tP;//创建一个指向数组的指针，该数组中存放的是指向链表头的指针
    int varNum = 0;
    int claNum = 0;
    //创建一个指向数组的指针，该数组中存放的是指向变量的真假赋值，1表示真，-1表示假，0表示未赋值
    int * varArrP;
}CnfData;

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

void Display(int n)//对求解器给出的N皇后问题的结果进行可视化展示
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
            int num = stoi(buf.substr(start,end-start));
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

CnfData* LoadCnfData(string fileName)//用邻接表来存放CNF文件中的数据，返回指向该邻接表的指针
{
    ifstream ifs(fileName,ios::in);
    if(ifs.is_open())
    {
        CnfData * cnfData = new CnfData;
        int cnt = 1;//标记记录到了第几条子句
        string buf;
        getline(ifs,buf);
        if(buf[0]=='p')//如果读到信息行
        {
            int space1 = buf.find(' ',6);//从信息行的下标6处开始提取变量数
            cnfData->varNum = stoi(buf.substr(6,space1-6));
            int space2 = buf.find(' ',space1+1);//提取子句数
            cnfData->claNum = stoi(buf.substr(space1+1,space2-space1-1));
            cnfData->tP = new Node * [cnfData->claNum];//创建数组
            cnfData->varArrP = new int[cnfData->varNum+1];//创建变量的真假赋值数组，该数组首位空出
            for(int i=0;i<cnfData->varNum+1;i++)
                cnfData->varArrP[i] = 0;
            while(cnt<=cnfData->claNum)//子句没有全部读完前循环
            {
                cnfData->tP[cnt-1] = new Node;//为每条子句创建链表头节点
                cnfData->tP[cnt-1]->next = NULL;
                Node *rear = cnfData->tP[cnt-1];//建立一个尾指针，先指向头节点
                getline(ifs,buf);
                // cout<<buf<<endl;
                int start = 0,end;
                int varSize = 0;//记录当前读到了几个变量
                while(1)
                {
                    end = buf.find(' ',start);
                    // cout<<stoi(buf.substr(start,end-start))<<endl;
                    if(stoi(buf.substr(start,end-start))!=0)//如果没有读到0，为每个变量创建新节点
                    {
                        rear->next = new Node;
                        rear->next->val = stoi(buf.substr(start,end-start));
                        rear->next->next = NULL;
                        rear = rear->next;
                    }
                    else//读到0的话不作为，rear->next==NULL，链表结束
                        break;
                    varSize++;
                    start = end+1;
                }
                cnfData->tP[cnt-1]->val = varSize;//把这个子句的变量数读写入头节点
                cnt++;
            }     
        }
        return cnfData;
    }
    else
    {
        cout<<"文件打开失败"<<endl;
        return NULL;
    }
}

bool SearchUnit(CnfData *data,int &unitCla)//找单子句，找到的话就通过参数传回该单子句中的文字
{
    for(int i=0;i<data->claNum;i++)
    {
        if(data->tP[i]->val==1)
        {
            unitCla = data->tP[i]->next->val;
            return true;
        }
    }
    return false;
}

void Assign(CnfData *data,int var)//根据变量正负对某进行赋值
{
    data->varArrP[(var>0)?var:-var] = (var>0)?1:-1;
}

void UnitPropagate(CnfData *data,int unitCla)//单元传播
{
    if(data->varArrP[(unitCla>0)?unitCla:-unitCla]==0)//如果该单子句还未赋值，就给它赋真值
        Assign(data,unitCla);

    // cout<<"-------------------------------------------------"<<endl;
    // cout<<"The assignments of each variables are as follows:"<<endl;
    // for(int i=1;i<=data->varNum;i++)
    //     cout<<i<<" : "<<data->varArrP[i]<<"  ";
    // cout<<endl<<endl;
    // cout<<"-------------------------------------------------"<<endl;

    int newClaNum = data->claNum;//标记新的子句数
    for(int i=0;i<data->claNum;i++)
    {
        Node *pNode = data->tP[i];//工作指针指向子句的头节点，准备逐个读取
        while(pNode->next!=NULL)
        {
            if(pNode->next->val==unitCla)//找某子句是否有同样的文字
            {
                data->tP[i]->val = -1;//先标记，等下一起整合
                data->tP[i]->next = NULL;
                newClaNum--;
                break;
            }
            if(pNode->next->val==-unitCla)//找某子句是否有相应的负文字
            {
                pNode->next = pNode->next->next;//物理删除该文字
                data->tP[i]->val--;
            }
            if(pNode->next!=NULL)
                pNode = pNode->next;
        }
    }
    
    if(newClaNum!=0)
    {
        Node ** p = new Node * [newClaNum];//创建新数组
        for(int i=0,j=0;i<data->claNum;i++)//把未删除的子句的头节点指针放入新数组
        {
            if(data->tP[i]->val!=-1)
            {
                p[j] = data->tP[i];
                j++;
            }
        }
        delete [] data->tP;//释放旧数组空间
        data->tP = p;
        data->claNum = newClaNum;
    }
    else
    {
        delete [] data->tP;//释放旧数组空间
        data->tP = NULL;
        data->claNum = 0;
    }
}

bool SetIsEmpty(CnfData *data)//检查集合是否为空
{
    return data->claNum==0;
}

bool ExistEmptySet(CnfData *data)//检查是否存在空子句
{
    for(int i=0;i<data->claNum;i++)
    {
        if(data->tP[i]->val==0)
        {
            // cout<<"The "<<i+1<<"th is empty set"<<endl;
            Node *pNode = data->tP[i];
            while(pNode->next!=NULL)
            {
                cout<<pNode->next->val;
                pNode = pNode->next;
            }
            cout<<endl;

            return true;
        }
    }
    return false;
}

int PickBranchVar(CnfData *data)//选择一个变量进行分裂
{
    for(int i=1;i<=data->varNum;i++)//这里就简单地挑第一个未被赋值的变量
        if(data->varArrP[i]==0)
            return i;
}

CnfData *AddUnitCla(CnfData *data,int var)//在一组子句中加入一个新的单子句
{
    CnfData *newData = new CnfData;//创建新的邻接表来存储新子句集合
    newData->claNum = data->claNum + 1;//多加一条子句
    newData->varNum = data->varNum;
    newData->tP = new Node * [newData->claNum];
    newData->varArrP = new int[newData->varNum];
    for(int i=0,j=0;i<data->claNum;i++,j++)//先把原来的数据拷贝进来
    {
        //i和j分别指向旧数据和新数据当前要处理的是第几条子句
        Node *oldPNode = data->tP[i], *newPNode;
        if(oldPNode->val==0)//如果原来的子句是空子句，保留头节点即可
        {
            newData->tP[j] = new Node;//先创建头节点
            newData->tP[j]->val = 0;
            newData->tP[j]->next = NULL;
        }
        else//如果原来的子句不空，逐个复制
        {
            newData->tP[j] = new Node;//先创建头节点
            newPNode = newData->tP[j];
            while(oldPNode->next!=NULL)
            {
                newPNode->val = oldPNode->val;
                newPNode->next = new Node;
                oldPNode = oldPNode->next;
                newPNode = newPNode->next;
            }
            newPNode->val = oldPNode->val;
            newPNode->next = NULL;
        }
    }
    newData->tP[data->claNum] = new Node;//对新加入的单子句进行处理
    newData->tP[data->claNum]->val = 1;
    newData->tP[data->claNum]->next = new Node;
    newData->tP[data->claNum]->next->val = var;
    newData->tP[data->claNum]->next->next = NULL;
    //然后处理真假值表
    for(int i=0;i<=data->varNum;i++)//先复制
        newData->varArrP[i] = data->varArrP[i];

    return newData;
}

void saveSol(CnfData *data,int *solP)//把当前变量的真假赋值保存到一个新的数组中
{
    for(int i =1;i<=data->varNum;i++)
        solP[i] = data->varArrP[i];
}

void printOneSol(int *solP)//输出一个解
{
    for(int i=1;i<=solP[0];i++)
    {
        if(solP[i]==-1)
            cout<<-i;
        else//未赋值的按赋真值处理
            cout<<i;
        cout<<" ";
    }
    cout<<endl;
}

bool DPLLSolver(CnfData *data,int *solP)//基于DPLL的求解器
{ 
    while(1)//找单子句，进行单元传播，找不到就进行分裂
    {
        int unitCla;
        if(SearchUnit(data,unitCla))//找单子句
        {
            UnitPropagate(data,unitCla);//进行单元传播
            if(SetIsEmpty(data))
            {
                saveSol(data,solP);//说明到这里是可满足的，把当前变量的真假赋值保存下来
                return true;
            }
            else if(ExistEmptySet(data))
                return false;
        }
        else//找不到就进行分裂
        {
            int branchVar = PickBranchVar(data);
            return( DPLLSolver(AddUnitCla(data,branchVar),solP) || DPLLSolver(AddUnitCla(data,-branchVar),solP) );
        }
    }
}

int main()
{
    CreateNQueensFile(N);
    
    CnfData *data = LoadCnfData(to_string(N)+"-Queen.cnf");//先读取CNF文件数据
    int *solP = new int[data->varNum+1];//用一个数组来存放变量的赋值
    solP[0] = data->varNum;//新数组首位同样不放变量数据，这里用来保存变量个数
    ofstream ofs(to_string(N)+"-out.txt",ios::out);//把结果保存到文件中
    if(DPLLSolver(data,solP))
    {
        cout<<"SAT"<<endl;
        printOneSol(solP);
        ofs<<"SAT"<<endl;
        for(int i=1;i<=solP[0];i++)
        {
            if(solP[i]==-1)
                ofs<<-i;
            else//未赋值的按赋真值处理
                ofs<<i;
            ofs<<" ";
        }
        ofs<<endl;
    }
    else
    {
        cout<<"UNSAT"<<endl;
        ofs<<"UNSAT"<<endl;
    }
    ofs.close();

    Display(N);
    // cout<<"p cnf "<<data->varNum<<" "<<data->claNum<<endl;
    // for(int i=0;i<data->claNum;i++)
    // {
    //     Node *pNode = data->tP[i];//工作指针指向子句的头节点，准备逐个读取
    //     if(pNode->val!=-1)//先检查该子句是不是已经逻辑删除
    //     {
    //         while(pNode->next!=NULL)
    //         {
    //             cout<<pNode->next->val<<" ";
    //             pNode = pNode->next;
    //         }
    //         cout<<"0  varSize:"<<data->tP[i]->val <<endl;
    //     }  
    // }

    system("pause");
    return 0;
}
