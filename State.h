//
// Created by Cristi on 11/14/2016.
//
#include <vector>
#include <set>
#include <map>
#include <unistd.h>
#include <cmath>
#include <stdlib.h>


struct block{
    short number;
    short x,y;
    block(short x, short y, short number)
    {
        this-> number = number;
        this-> x = x;
        this-> y = y;
    }
    block(block *b)
    {
        this->number = b->number;
        this->x = b->x;
        this->y = b->y;
    }
    block()
    {
        this->number = 1;
        this-> x = 0;
        this-> y = 0;
    };
};

class State
{
private:
    std::map<std::pair<short,short>, short> blockMap;
    std::map<std::pair<short,short>, short> obstMap;
    std::map<short,std::pair<short,short> > invertedBlockMap;
    block agent;
    short size;
    short steps;
    std::pair<short,short> previousMove;
    State* parent;
    int imp;

public:
    State(std::vector<block> objects, block agent, short size);
    State(std::vector<block> objects, block agent, short size, std::vector<block> obstacles);
    State(std::map<std::pair<short,short>,short> *mp, block *agent, short size, short steps, State* parent,std::map<std::pair<short,short>,short> *obsMap);
    State(){}
    block* getAgent();
    std::map<std::pair<short,short>,short>* getBlockMap();
    std::string hash();
    State* moveAgent(short x,short y);
    bool check();
    void print();
    short getSteps();
    short getSize();
    int getImp();
    void computeImp();
    void setParent(State*);
    State* getParent();
    void destroy();
    void setPreviousMove(short,short);
    void printConfiguration();
    std::map<std::pair<short,short>,short>* getObstMap();
    std::pair<short,short> getPreviousMove();

};

State::State(std::vector<block> objects, block agent, short size)
{
    //std::cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
    for(auto bl : objects)
    {
        //std::cout<<bl.y<<" "<<bl.x<<" "<<bl.number<<std::endl;
        this->blockMap.insert(std::make_pair(std::make_pair(bl.y,bl.x),bl.number));
    }
    //std::cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
    this->agent = agent;

    this->size = size;

    this->steps = 0;

    this->parent = this;

};

State::State(std::map<std::pair<short,short>,short> *mp, block *agent, short size, short steps, State* parent, std::map<std::pair<short,short>,short> *obsMp){
    //std::cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@ after\n";
    for(auto bl : *mp)
    {
        //std::cout<<bl.first.first<<" "<<bl.first.second<<" "<<bl.second<<std::endl;
        blockMap[bl.first] = bl.second;
    }
    this->steps = steps;
    for(auto bl : *obsMp)
    {
        obstMap[bl.first] = bl.second;
        std::cout<<bl.first.first<<" "<<bl.first.second<<" "<<this->steps<<"\n";
    }
    //std::cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
    this->agent = new block(agent);
    this->size = size;
    this->parent = parent;
};

State::State(std::vector<block> objects, block agent, short size, std::vector<block> obstacles)
{
    //std::cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
    for(auto bl : objects)
    {
        //std::cout<<bl.y<<" "<<bl.x<<" "<<bl.number<<std::endl;
        this->blockMap.insert(std::make_pair(std::make_pair(bl.y,bl.x),bl.number));
    }
     for(auto bl : obstacles)
    {
        this->obstMap.insert(std::make_pair(std::make_pair(bl.y,bl.x),'1'));
        std::cout<<bl.y<<" "<<bl.x<<"\n";
    }
    //std::cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
    this->agent = agent;

    this->size = size;

    this->steps = 0;

    this->parent = this;

};

std::map<std::pair<short,short>,short>* State::getBlockMap()
{
    return &blockMap;
}


std::map<std::pair<short,short>,short>* State::getObstMap()
{
    return &obstMap;
}

block* State::getAgent()
{
    return &agent;
}

std::string State::hash()
{
    std::string toReturn = "";

    for(auto block : this->blockMap)
    {
        //adding only the coordinates of blocks
        toReturn += block.first.second;
        toReturn += " ";
        toReturn += block.first.first;
        toReturn += " ";
        toReturn += block.second;
        toReturn += " ";
    }

    toReturn += this->agent.x;
    toReturn += " ";
    toReturn += this->agent.y;
    return toReturn;
}

State* State::moveAgent(short y,short x)
{
    agent.x +=x;
    agent.y +=y;

    State *newState;

    //returning if agent is on an obstacle or out of bounds
    if(agent.x > size || agent.x < 1 || agent.y > size || agent.y < 1 || obstMap[std::make_pair(agent.y,agent.x)]==1)
    {
        agent.x -=x;
        agent.y -=y;
        newState = new State(&blockMap,&agent,this->size,this->steps,this,&obstMap);
        return newState;
    }else
    {
        //if we checked the last condition and it was false, it created a new element in obstMap that should be erased
        obstMap.erase(std::make_pair(agent.y,agent.x));
    }

    std::pair<short,short> p = std::make_pair(agent.y,agent.x);
    std::map<std::pair<short,short>, short>::iterator it =  blockMap.find(p);
    //this->printConfiguration();


    //creating the new State:
    if(it != blockMap.end())
    {
        //if the new state need to swap between agent and a block
        std::map<std::pair<short,short>, short> blockMap2 = blockMap;
        std::map<std::pair<short,short>, short>::iterator it2 = blockMap2.find(p);

        blockMap2.erase(it2);

        p.first -= y;
        p.second -= x;

        blockMap2[p] = it->second;
        newState = new State(&blockMap2,&agent,size,this->steps+1,this,&obstMap);

    }else
    {
        //if the new state doesn't need to swap between agent and blocks
        std::map<std::pair<short,short>, short> blockMap2 = blockMap;
        newState = new State(&blockMap2,&agent,size,this->steps+1,this,&obstMap);
    }

    newState->setPreviousMove(y,x);

    //undoing the changes we have made on the agent
    agent.x -=x;
    agent.y -=y;

    return newState;
}

void State::print()
{
    short v[this->size+1][this->size+1];
    std::pair<short,short> p;
    short N = this->size;

    for(short i = 1; i <= N; ++i)
    {
        for(short j = 1; j <= N; ++j)
        {
            v[i][j]=0;
        }
    }

    for(auto it : blockMap)
    {
        //std::cout<<it.first.first<<" "<<it.first.second<<" "<<it.second<<std::endl;
        v[it.first.first][it.first.second] = it.second + (int)'A' -1;
    }
    v[agent.y][agent.x] = (short)'#';
    for(auto it : obstMap)
    {
        //std::cout<<it.first.first<<" "<<it.first.second<<" "<<it.second<<std::endl;
        //std::cout<<"printing obstacles";
        v[it.first.first][it.first.second] = '1';
    }
    for(short i = 1; i <= N; ++i)
    {
        for(short j = 1; j <= N; ++j)
        {
            if(v[i][j]==0)
                std::cout<<"0 ";
            else std::cout<<char(v[i][j])<<" ";
        }
        std::cout<<std::endl;
    }
   // std::cout<<"Hash: "<<this->hash()<<std::endl;

}

bool State::check()
{
    std::pair<short,short> p;

    //Finds the last block (e.g. if the size is 4, it finds the block 'C')
    for(auto block : this->blockMap)
    {
        if(block.second == this->size-1)
        {
            p = block.first;
            break;
        }
    }

    //if the last block is not on the last 2 rows, return false
    if(p.first < this->size - 1)
        return false;

    //returns true if everything is okay, or false if the order is not correct
    for(short i = 2; i < this->size; ++i)
    {
        p.first--;
        if(blockMap[p]!=this->size-i)
        {
            if(blockMap[p]==0)
                blockMap.erase(p);
            return false;
        }
    }

    return true;
}

short State::getSteps()
{
    return steps;
}

short State::getSize()
{
    return this->size;
}

void State::setPreviousMove(short y, short x)
{
    previousMove.first = y;
    previousMove.second = x;
}


std::pair<short,short> State::getPreviousMove()
{
    return previousMove;
}

State* State::getParent()
{
    return this->parent;
}

void State::destroy()
{
    delete(&blockMap);
    delete(&agent);
    delete(&steps);
    delete(&size);
}

void State::printConfiguration()
{
    std::cout<<"<---------------------------->\n\n";
    std::cout<<"Agent line = "<<agent.y<<"; and column = "<<agent.x<<std::endl;
    std::cout<<"Size = "<<size<<std::endl;
    std::cout<<"Level = "<<steps<<std::endl;
    std::cout<<"\n<---------------------------->\n";
}

int State::getImp()
{
    return this->imp;
}

void State::setParent(State* parent)
{
    this->parent = parent;
}

void State::computeImp()
{
    //First of all, creating a new map with the elements of the first map but inverted.
    for(auto bl : blockMap){
        invertedBlockMap[bl.second] = bl.first;
    }
    //initializing minimumDist with a very large number;
    int minimumDist=999999999,currentDist;
    for(int j = 1; j<=size; ++j)
    {
        currentDist=0;
        //computing the distance for solutions that begin on last row
        for(int i = size; i>1; --i)
        {
            currentDist+= abs(invertedBlockMap[i-1].first - i) + abs(invertedBlockMap[i-1].second - j);
            /*if(invertedBlockMap[i-1].second == j && invertedBlockMap[i-1].first != i)
            {
                currentDist++;
            }*/
        }
        //always selecting the minimum
        minimumDist = std::min(currentDist,minimumDist);

        currentDist=0;
        //computing the distance for solutions that begin on last-1 row
        for(int i = size-1; i>0; --i)
        {
            currentDist+= abs(invertedBlockMap[i].first - i) + abs(invertedBlockMap[i].second - j);
            /*if(invertedBlockMap[i].second == j && invertedBlockMap[i].first != i)
            {
                currentDist++;
            }*/
        }
        //always selecting the minimum
        minimumDist = std::min(currentDist,minimumDist);
    }

    this->imp = minimumDist;
}

