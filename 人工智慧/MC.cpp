#include<iostream>
#include<list>
#include<vector>
#include <cassert>
#include<iterator>
#include <fstream>
#include <cmath>

using namespace std;
ofstream ofs;
//global
int totalNumCann;
int totalNumMiss;

class State {

  private:
    vector<int> state; // < number of cannibals, number of missionaries, boat1 position,  boat2 position>
    State *parent;
    int gn;            //從root節點到此節點的實際距離
    int hn;            //從此節點到目的地的預測距離

  public:
    State (int numCann, int numMiss, int boat1, int boat2); //船左邊為1 

    bool isLegalState ();
    bool isGoalState () const;
    void printState ();

    bool operator==(const State & rhs) const{
        return ((state == rhs.state) && (*parent == *rhs.parent));
    }
    void sethn(int n){
        hn = n;
    }
    int gethn(){
        return hn;
    }
    void setgn(int n){
        gn = n;
    }
    int getgn(){
        return gn;
    }
    State *getParent(){
        return parent;
    };
    void setParent(State *s);
    vector<int> getStateAttr();
};

State::State (int numCann, int numMiss, int boat1, int boat2){
    //assert(numCann >= 0 && numMiss >= 0);
    state.push_back(numCann);
    state.push_back(numMiss);
    state.push_back(boat1);
    state.push_back(boat2);
}

bool State::isLegalState () {
    if(state[0] < 0 || state[1] < 0 || state[0] > totalNumCann || state[1] > totalNumMiss) return false;
    bool legalLeftBankState = true;
    if(state[1] > 0) legalLeftBankState = state[1] >= state[0];

    bool legalRightBankState = true;
    if(totalNumMiss - state[1] > 0){
        legalRightBankState = totalNumMiss - state[1] >= totalNumCann - state[0];
    }

    return (legalLeftBankState && legalRightBankState);
}

bool State::isGoalState() const{
    if((state[0] == 0) && (state[1] == 0)){
        return true;
    }
    return false;
}

vector<int> State::getStateAttr(){
    return state;
}
void State::setParent(State *s){
    parent = s;
}

//放進OPEN list的排序函式
bool op(State* lhs, State* rhs) {
        return lhs->getgn() < rhs->getgn(); 
}

bool astarop(State* lhs, State* rhs) {
        return lhs->getgn() + lhs->gethn() < rhs->getgn() + rhs->gethn(); 
}

vector<State*> nextState(State *start){
    vector<int> state = start->getStateAttr();
    int lc = state[0];
    int lm = state[1];
    int boat1 = state[2];
    int boat2 = state[3];
    vector<State*> next;
    for (int Am=0;Am<=2;Am++){     //船A
        for (int Ac=0;Am + Ac<=2;Ac++){
            for (int Bm=0;Bm<=3;Bm++){   //船B
                for (int Bc=0;Bm + Bc<=3 ;Bc++){
                    if((Bm+Bc == 0)){           //只有A船開
                        if((Am+Ac>=1) && (Am+Ac<=2) && (Am>=Ac||(Ac>0))){
                           // cout << "AAAAA"<<endl;
                            State* s = new State(lc - boat1 * Ac, lm - boat1 * Am, -boat1, boat2);
                            if(s->isLegalState()){
                                s->setgn(start->getgn() + 3);
                                int hn = abs(lc - boat1 * Ac) + abs(lm - boat1 * Am);
                                s->sethn(hn);
                                s->setParent(start);
                                next.push_back(s);
                            }
                        }
                    }
                    else if((Am+Ac == 0)){           //只有B船開
                        if((Bm+Bc>=1) && (Bm+Bc<=3) && (Bm>=Bc||(Bc>0 && Bm==0))){
                           // cout << "BBBBB"<<endl;
                            State* s = new State(lc - boat2 * Bc, lm - boat2 * Bm, boat1, -boat2);
                            if(s->isLegalState()){
                                s->setgn(start->getgn() + 25);
                                int hn = abs(lc - boat2 * Bc) + abs(lm - boat2 * Bm);
                                s->sethn(hn);
                                s->setParent(start);
                                next.push_back(s);
                            }
                        }
                    }
                    // A and B 
                    else if(((Am+Ac>=1) && (Am+Ac<=2) && (Bm+Bc>=1) && (Bm+Bc<=3))){
                        if((Am>=Ac||(Ac>0))&&(Bm>=Bc||(Bc>0 && Bm==0))){
                           // cout <<"ABABABA" <<endl;
                            State* s = new State(lc - boat2 * Bc - boat1 * Ac, lm - boat2 * Bm -boat1 * Am, -boat1, -boat2);
                            if(s->isLegalState()){
                                s->setgn(start->getgn() + 28);
                                int hn = abs(lc - boat2 * Bc - boat1 * Ac) + abs(lm - boat2 * Bm -boat1 * Am);
                                s->sethn(hn);
                                s->setParent(start);
                                next.push_back(s);
                            }
                        }
                    }
                }
            }
        }
    }
    return next;
}

bool notFound(State* start, list<State*> & OPEN, list<State*> & CLOSE){
    list<State*>::iterator itr1 = OPEN.begin();
    list<State*>::iterator itr2 = CLOSE.begin();
    for(; itr1 != OPEN.end() ; itr1++){
        if( ((*itr1)) == (start) ) break;
    }
    for(; itr2 != CLOSE.end(); itr2++){
        if( ((*itr2)) == (start) ) break;
    }

    if( (itr1 == OPEN.end()) && (itr2 == CLOSE.end()) )
        return true;                           
    return false;
}

State* dijkstra(list<State*> & OPEN, list<State*> & CLOSE){     //求價格最低

    //ofs.open("output.txt");
    while(!OPEN.empty()){
        OPEN.sort(op);
        State *root;
        root = OPEN.front();
        ofs<<"----------------------------------"<<endl;
        cout<<"----------------------------------"<<endl;
        ofs << root->getStateAttr()[0] << " " << root->getStateAttr()[1] << " "<< root->getStateAttr()[2] << " " << root->getStateAttr()[3] << endl;
        cout << root->getStateAttr()[0] << " " << root->getStateAttr()[1] << " "<< root->getStateAttr()[2] << " " << root->getStateAttr()[3] << endl;
        if(root->isGoalState()){
            return root;
        }
        OPEN.pop_front();
        CLOSE.push_back(root);
        vector<State*> vec = nextState(root);
        ofs << "end of one next state: " << vec.size() << endl;
        cout << "end of one next state: " << vec.size() << endl;
        vector<int> ini;
        ini.push_back(totalNumCann);
        ini.push_back(totalNumMiss);
        ini.push_back(1);
        ini.push_back(1);
        for(int i=0; i<vec.size();i++){
            //cout << i <<endl;
            if(vec[i]->isLegalState()  && notFound(vec[i], OPEN, CLOSE) && vec[i]->getStateAttr() != ini){
          //      cout<< "ok is valid" << endl;
                OPEN.push_back(vec[i]);
            }
        }
    }
    //ofs.close();
    return nullptr;
}

State* astar(list<State*> & OPEN, list<State*> & CLOSE){     //求最短路徑
    while(!OPEN.empty()){
        OPEN.sort(astarop);
        State *root;
        root = OPEN.front();
        ofs<<"----------------------------------"<<endl;
        cout<<"----------------------------------"<<endl;
        ofs << root->getStateAttr()[0] << " " << root->getStateAttr()[1] << " "<< root->getStateAttr()[2] << " " << root->getStateAttr()[3] << endl;
        cout << root->getStateAttr()[0] << " " << root->getStateAttr()[1] << " "<< root->getStateAttr()[2] << " " << root->getStateAttr()[3] << endl;
        if(root->isGoalState()){
            return root;
        }
        OPEN.pop_front();
        CLOSE.push_back(root);
        vector<State*> vec = nextState(root);
        ofs << "end of one next state: " << vec.size() << endl;
        cout << "end of one next state: " << vec.size() << endl;
        vector<int> ini;
        ini.push_back(totalNumCann);
        ini.push_back(totalNumMiss);
        ini.push_back(1);
        ini.push_back(1);

        for(int i=0; i<vec.size();i++){
            if(vec[i]->isLegalState() && notFound(vec[i], OPEN, CLOSE) && vec[i]->getStateAttr() != ini){
                OPEN.push_back(vec[i]);
            }
        }
    }
}

State* bfs(list<State*> & OPEN, list<State*> & CLOSE){     //求最短路徑
    while(!OPEN.empty()){
        State *root;
        root = OPEN.front();
        ofs<<"----------------------------------"<<endl;
        cout<<"----------------------------------"<<endl;
        ofs << root->getStateAttr()[0] << " " << root->getStateAttr()[1] << " "<< root->getStateAttr()[2] << " " << root->getStateAttr()[3] << endl;
        cout << root->getStateAttr()[0] << " " << root->getStateAttr()[1] << " "<< root->getStateAttr()[2] << " " << root->getStateAttr()[3] << endl;
        if(root->isGoalState()){
            return root;
        }
        OPEN.pop_front();
        CLOSE.push_back(root);
        vector<State*> vec = nextState(root);
        ofs << "end of one next state: " << vec.size() << endl;
        cout << "end of one next state: " << vec.size() << endl;
        vector<int> ini;
        ini.push_back(totalNumCann);
        ini.push_back(totalNumMiss);
        ini.push_back(1);
        ini.push_back(1);

        for(int i=0; i<vec.size();i++){
            if(vec[i]->isLegalState() && notFound(vec[i], OPEN, CLOSE) && vec[i]->getStateAttr() != ini){
                OPEN.push_back(vec[i]);
            }
        }
    }
}

int main(){
    ofs.open("log.txt");
    int bfs_or_dij;
    int tCann, tMiss;
    cout << "plz input amount of cannible and missionary:";
    cin >> tCann >> tMiss;
    totalNumCann = tCann;
    totalNumMiss = tMiss;
    State* A = new State(tCann, tMiss, 1, 1);
    list<State*> OPEN, CLOSE;
    OPEN.push_back(A);
    cout << "bfs type 1, dijkstra type 2, astar type 3:";
    cin >> bfs_or_dij;
    State *final;
    if(bfs_or_dij==1){
        final = bfs(OPEN, CLOSE);
    }
    else if(bfs_or_dij == 2){
        final = dijkstra(OPEN, CLOSE);
    }
    else{
        final = astar(OPEN, CLOSE);
    }

    if(final == nullptr){
        cout << "can't find" << endl;
    }
    else{
        //cout <<"test!!"<<endl;
        vector<vector<int> > final_list;
        State* temp = final;
        while(temp != A){
            final_list.push_back(temp->getStateAttr());
            temp = temp->getParent();
        }
        final_list.push_back(A->getStateAttr());
        for(int i = final_list.size() - 1;i>=0;i--){
            ofs << final_list[i][0] << " " << final_list[i][1] << " " << final_list[i][2] << " " << final_list[i][3] << endl;
            cout << final_list[i][0] << " " << final_list[i][1] << " " << final_list[i][2] << " " << final_list[i][3] << endl;
        } 
        cout << "cost: " << final->getgn();
        // for(int i = final_list.size() - 1;i>=0;i--){
        //     for (int j = 0;j<final_list[])
        // }
    }
    ofs.close();
}