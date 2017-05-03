#include <iostream>
#include <vector>
#include <map>
#include <stdlib.h>
#include <time.h>
#include "State.h"
#include "TreeSearcher.h"

using namespace std;

int N;
int newX,newY;
char type;
char srch;
char rev;
char ranBr;
bool ranBool;
bool revBool;
map<pair<int,int>, char> mp;
vector<block> blocks;

int main()
{
    cout<<"Select search method:\n1 BFS;\n2 DFS;\n3 Iterative Deepening;\n4 A*\n";
    while(true){
        cin>>type;
        if(type == '1' || type == '2' || type == '3' || type == '4')
            break;
        else
            cout<<"\nInvalid search method. Try again:\n";
    }
    cout<<"Graph search or Tree search? Type 1 or 2:";
    while(true){
        cin>>srch;
        if(srch == '1' || srch == '2')
            break;
        else
            cout<<"\nInvalid search type. Try again:\n";
    }
    cout<<"\nShould I include reversing moves? ('no' -> for example if the agent goes up, it can't go down as a next move)\n.Type 'y' or 'n' for yes or no:\n";
    while(true){
        cin>>rev;
        if(rev == 'y' || rev == 'n')
        {
            if(rev == 'y')
                revBool = true;
            else revBool = false;
            break;
        }
        else
            cout<<"\nInvalid answer. Try again ('y' or 'n'):\n";
    }
    cout<<"\nRandom order of expanding nodes? 'y' or 'n'\n";
    while(true){
        cin>>ranBr;
        if(ranBr == 'y' || ranBr == 'n')
        {
            if(ranBr == 'y')
                ranBool = true;
            else ranBool = false;
            break;
        }
        else
            cout<<"\nInvalid answer. Try again ('y' or 'n'):\n";
    }
    cout<<"Introduce number of blocks:";
    cin>>N;
    //reading objects:
    int cn = N;
    ++N;
    cout<<endl<<"Where should the blocks be? (Don't introduce invalid values)\n";
    char c = 'A';
    block *p;
    do
    {
        cout<<"For "<<c<<":"<<endl;
        cin>>newY>>newX;

        if(newX>N+1||newX<1||newY>N+1||newY<1){
            cout<<endl<<"The coordinates you tried are invalid. Try again."<<endl;
            continue;
        }

        if(mp[make_pair(newX,newY)]!=0){
            cout<<endl<<"The coordinates you tried are occupied by "<<mp[make_pair(newX,newY)]<<" .Try again."<<endl;
            continue;
        }

        mp[make_pair(newX,newY)] = c;
        p = new block(newX,newY,c-'A'+1);
        blocks.push_back(*p);
        p = new block;

        c++;
        cn--;
    }while(cn);

    cout<<"Where should the agent be?\n";
    block *agent = new block;
    cin>> agent->y;
    cin>> agent->x;
    agent->number = 0;

    char obs;
    cout<<"Do you want obstacles? ('y' or 'n')";
    cin>>obs;
    if(obs=='y')
    {
        vector<block> obstacles;
        int nrObs;
        cout<<"How many obstacles?\n";
        cin>>nrObs;
        while(nrObs--){
            cin>>newY>>newX;
            if(mp[make_pair(newX,newY)]!=0){
            cout<<endl<<"The coordinates you tried are occupied by "<<mp[make_pair(newX,newY)]<<" .Try again."<<endl;
            nrObs++;
            continue;
            }
            p = new block(newX,newY,'1');
            obstacles.push_back(*p);
        }
        State *startState = new State(blocks,*agent,N,obstacles);
        TreeSearcher *tr = new TreeSearcher(startState,type,srch,true,ranBool);
    }else{
        State *startState = new State(blocks,*agent,N);
        TreeSearcher *tr = new TreeSearcher(startState,type,srch,true,ranBool);
    }
    return 0;
}
