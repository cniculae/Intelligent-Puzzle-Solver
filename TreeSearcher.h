//
// Created by Cristi on 11/15/2016.
//
#include <map>
#include <queue>
#include <stack>
#include <unistd.h>
#include <functional>


//Overriding '<' operator for the priority queue:

class Compare
{
public:
    bool operator() (State* a, State* b)
    {
        return a->getImp()>b->getImp();
    }
};


class TreeSearcher
{
private:
    std::map<std::string,int> mp;
    std::queue<State*> queue;
    std::stack<State*> stack;
    std::priority_queue<State*,std::vector<State*>,Compare> pq;
    bool graphSearch;
    long long exteriorNodes = 0;
    //1 - BFS, 2 - DFS, 3 - iterative deepening, 4 - A*
    char type;
    bool rev;
    bool randomBr;
public:
    TreeSearcher(State*, char, char, bool,bool);
    void expand(State state);
    bool check(State* state);
    void startBFS();
    bool startDFS(int);
    void startAStar();
    void finalAll(State*);
};

TreeSearcher::TreeSearcher(State *startState, char type, char graphSearch, bool rev, bool ran)
{
    //Initializing options for tree search
    if(graphSearch == '1'){
        this->graphSearch = true;
        std::cout<<"da";
    }
    else
        this->graphSearch = false;
    this->type = type;
    this->rev = rev;
    this->randomBr = ran;
    srand(time(NULL));

    if(type == '1')
    {
        // type 1 -> BFS
        mp[startState->hash()]=1;
        startState->setParent(startState);
        queue.push(startState);
        startState->printConfiguration();
        this->startBFS();
    }

    if(type == '2')
    {
        // type 2 -> DFS
        mp[startState->hash()]=1;
        stack.push(startState);
        startState->printConfiguration();
        this->startDFS(0);
    }

    if(type == '3')
    {
        // type 3 -> IDS
        mp[startState->hash()]=1;
        State* copyOfStart = new State(startState->getBlockMap(),startState->getAgent(),startState->getSize(),0,NULL, startState->getObstMap());
        stack.push(copyOfStart);
        startState->printConfiguration();
        int limit = 1;
        while(!(this->startDFS(limit++))){
            mp.clear();
            copyOfStart = new State(startState->getBlockMap(),startState->getAgent(),startState->getSize(),0,NULL,startState->getObstMap());
            stack.push(copyOfStart);
            std::cout<<"New limit:"<<limit<<std::endl;
        };
    }

    if(type == '4')
    {
        //type 4 -> A*
        mp[startState->hash()]=1;
        pq.push(startState);
        startState->printConfiguration();
        this->startAStar();
    }
}

void TreeSearcher::startBFS()
{
    State *state;
    long long nodes=0;
    int maximSteps=-1;
    while(!queue.empty()){
        state = queue.front();
        ++nodes;
        queue.pop();
        if(maximSteps < state->getSteps())
        {
            maximSteps ++;
            std::cout<<"It took "<<nodes<<" nodes to reach level "<<maximSteps<<std::endl<<std::endl;
        }
        if(this->check(state)){
            std::cout<<"SOLUTION:\n";
            state->print();
            std::cout<<"\nIt took "<<nodes<<" nodes to find the solution.";
            break;
        }
        this->expand(*state);
        //delete(state);
        state->destroy();
    }
    this->finalAll(state);
}

//int limit -> used to limit the DFS so it can be used in IDS;
//if limit == 0 then there is no limit applied on DFS
bool TreeSearcher::startDFS(int limit)
{
    State *state;
    long long nodes=0;
    int maximSteps=-1;
    while(!stack.empty())
    {
        state = stack.top();
        ++nodes;
        ++exteriorNodes;
        stack.pop();
        if(maximSteps < state->getSteps())
        {
            maximSteps ++;
        }
        if(this->check(state)){
            std::cout<<"SOLUTION:\n";
            state->print();
            std::cout<<"\nIt took "<<exteriorNodes<<" nodes to find the solution.";
            return true;
        }
        if(limit == 0 || state->getSteps() < limit)
            this->expand(*state);
        //delete(state);
        //state->destroy();
    }
    return false;
}

void TreeSearcher::startAStar()
{
    State *state;
    long long nodes = 0;
    int maximSteps = -1;
    while(!pq.empty())
    {
        state = pq.top();
        state->print();
        std::cout<<std::endl<<"Manhattan Distance: "<<state->getImp()<<"\n<-------------->"<<std::endl<<std::endl;
        ++nodes;
        pq.pop();
        if(maximSteps < state->getSteps())
        {
            maximSteps ++;
            //std::cout<<"It took "<<nodes<<" nodes to reach level "<<maximSteps<<std::endl;
        }
        if(this->check(state)){
            std::cout<<"<------------------------------->\nSOLUTION:\n\n";
            state->print();
            std::cout<<"\nIt took "<<nodes<<" nodes to find the solution.";
            break;
        }
        this->expand(*state);
    }

}

bool TreeSearcher::check(State* state)
{
    //state->print();
    return state->check();
}

void TreeSearcher::expand(State state)
{
    State *newState;
    mp[state.hash()]=1;
    std::pair<int,int> lastMove = state.getPreviousMove();
    //state.print();
    //std::cout<<"\n-----> "<<lastMove.first<<" "<<lastMove.second<<"\n";
    char up=0,down=0,left=0,right=0;
    char chanceIncr = 0;
    if(randomBr)
    {
        //Random expansions
        while(true)
        {
            if(!up && (rand() % 10) + chanceIncr > 8)
            {

                //Going up:
                if(rev || lastMove.first != 1)
                {
                    newState = state.moveAgent(-1,0);

                    if(!graphSearch || mp[newState->hash()]==0)
                        if(state.getSteps() != newState->getSteps())
                        {
                            switch(type){
                                case '1':queue.push(newState);break;
                                case '2':stack.push(newState);break;
                                case '3':stack.push(newState);break;
                                case '4':newState->computeImp();pq.push(newState);break;
                            }
                        }
                }
                up = 1;
                chanceIncr += 2;
            }

            //Going down:
            if(!down && (rand() % 10) + chanceIncr > 8)
            {
                //std::cout<<"down\n";
                if(rev || lastMove.first != -1)
                {
                    newState = state.moveAgent(1,0);
                    //std::cout<<newState->getSteps();
                    if(!graphSearch || mp[newState->hash()]==0)
                        if(state.getSteps() != newState->getSteps())
                        {
                            switch(type){
                                case '1':queue.push(newState);break;
                                case '2':stack.push(newState);break;
                                case '3':stack.push(newState);break;
                                case '4':newState->computeImp();pq.push(newState);break;
                            }
                        }
                }
                chanceIncr += 2;
                down = 1;
            }

            //Going right:
            if(!right && (rand() % 10) + chanceIncr > 8)
            {
                //std::cout<<"right\n";
                if(rev || lastMove.second != -1)
                {
                    newState = state.moveAgent(0,1);
                    if(!graphSearch || mp[newState->hash()]==0)
                        if(state.getSteps() != newState->getSteps())
                        {
                            switch(type){
                                case '1':queue.push(newState);break;
                                case '2':stack.push(newState);break;
                                case '3':stack.push(newState);break;
                                case '4':newState->computeImp();pq.push(newState);break;
                            }
                        }
                }
                chanceIncr += 2;
                right = 1;
            }
            //Going left:
            if(!left && (rand() % 10) + chanceIncr > 8)
            {
                //std::cout<<"left\n";
                if(rev || lastMove.second != 1)
                {
                    newState = state.moveAgent(0,-1);
                    if(!graphSearch || mp[newState->hash()]==0)
                        if(state.getSteps() != newState->getSteps())
                        {
                            switch(type){
                                case '1':queue.push(newState);break;
                                case '2':stack.push(newState);break;
                                case '3':stack.push(newState);break;
                                case '4':newState->computeImp();pq.push(newState);break;
                            }
                        }
                }
                chanceIncr += 2;
                left = 1;
            }
            if(up && down && left && right)
                break;
        }
    }else
    {
        //std::cout<<"up up to the stars\n";
        //Going up:
        if(rev || lastMove.first != 1)
        {
            newState = state.moveAgent(-1,0);
            if(!graphSearch || mp[newState->hash()]==0)
                if(state.getSteps() != newState->getSteps())
                {
                    switch(type){
                        case '1':queue.push(newState);break;
                        case '2':stack.push(newState);break;
                        case '3':stack.push(newState);break;
                        case '4':newState->computeImp();pq.push(newState);break;
                    }
                }
        }

        //Going down:
        //std::cout<<"down\n";
        if(rev || lastMove.first != -1)
        {
            newState = state.moveAgent(1,0);
            //std::cout<<newState->getSteps();
            if(!graphSearch || mp[newState->hash()]==0)
                if(state.getSteps() != newState->getSteps())
                {
                    switch(type){
                        case '1':queue.push(newState);break;
                        case '2':stack.push(newState);break;
                        case '3':stack.push(newState);break;
                        case '4':newState->computeImp();pq.push(newState);break;
                    }
                }
        }
        //Going right:
        //std::cout<<"right\n";
        if(rev || lastMove.second != -1)
        {
            newState = state.moveAgent(0,1);
            if(!graphSearch || mp[newState->hash()]==0)
                if(state.getSteps() != newState->getSteps())
                {
                    switch(type){
                        case '1':queue.push(newState);break;
                        case '2':stack.push(newState);break;
                        case '3':stack.push(newState);break;
                        case '4':newState->computeImp();pq.push(newState);break;
                    }
                }
        }
        //Going left:
        //std::cout<<"left\n";
        if(rev || lastMove.second != 1)
        {
            newState = state.moveAgent(0,-1);
            if(!graphSearch || mp[newState->hash()]==0)
                if(state.getSteps() != newState->getSteps())
                {
                    switch(type){
                        case '1':queue.push(newState);break;
                        case '2':stack.push(newState);break;
                        case '3':stack.push(newState);break;
                        case '4':newState->computeImp();pq.push(newState);break;
                    }
                }
        }
    }
    //delete(newState);
}

void TreeSearcher::finalAll(State* state)
{
    std::cout<<"\n\n Print the path of the tree? 'y' or 'n':\n";
    char y;
    while(true)
    {
        std::cin>>y;
        if(y=='y' || y=='n')
            break;
        std::cout<<"\nIncorrect option, please choose again ('y' or 'n'):\n";
    }
    if(y=='y')
    {
        while(state)
        {
            state->print();
            std::cout<<"Level:"<<state->getSteps()<<std::endl;
            std::cout<<std::endl;
            state = state->getParent();
        }
    }
}

