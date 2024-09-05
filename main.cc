#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <fstream>
#include <chrono>
#include <queue>
#include "chessmodule.h"
#include "server.h"
#include "client.h"

using namespace std;

int main(int argc, char** argv){
    // Initializing Chess
    cin.exceptions(ios::eofbit | ios::failbit);
    Server s;
    Client c;

    // We should include a helper function "usage" to handle when the user messes up the flag input
    unsigned seed = 0;
    ChessModule cm(seed, false);
    if (argc > 1) {
        int i = -1;
        if (string(argv[1]) == "-seed") {            
            i = 2;
            try{
                seed = std::stoi(string(argv[2]));
            }catch(...){
                std::cout << "seed parameter only accepts Integer. Abort program." << std::endl;
                return 0;
            }
        } else if (argc > 3 && string(argv[2]) == "-seed") {
            i = 3;
            try{
                seed = std::stoi(string(argv[3]));
            }catch(...){
                std::cout << "seed parameter only accepts Integer. Abort program." << std::endl;
                return 0;
            }
        } 
        if (string(argv[1]) == "-text") {
            i = 0;
            // to be implemented
            cm.setShowGraphicsDisplay(false);
        }
        if (i == -1) throw ChessException(); //seed is bad! Handle this case better please!!!
    } else {
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    cm.setSeed(seed);
    string cmd;
    ChessException ce;
    bool game_continuing = false;
    bool game_on = false; // if a game is started
    bool Whose_move; // false = white, true = black
    bool white_comp = false;
    bool black_comp = false;
    bool multiplayer_mode = false; // false = off, true = on
    bool server_active = false; // false = not active, true = active
    bool net_self_colour; // false = white, true = black; for network gameplay 
    bool host = false; // false = not a host, true = is a host; for network gameplay
    bool local_humans = false;
    bool loading = false;
    queue<string> load_queue;
    ofstream savelog;
    string savelog_name = "savelog.txt";
    vector<string> moves_so_far(0);
    int max_cpu_v_cpu_moves = 1000;
    int count = 0;
    try{
        while (true){
            if (white_comp && black_comp) { 
                ++count;
                if (count > max_cpu_v_cpu_moves) break; //lets you play computer vs computer without going forever
            }
            Whose_move = cm.getWhoseMove();
            if(cm.getWinner() == "white_checkmate"&&game_on){
                cout << "White won, Checkmate!" << endl;
                game_on = false;
                cm.clear_board();
                cm.addscore(false);
                if(server_active && multiplayer_mode){
                    multiplayer_mode = false;
                    break;
                }
            }else if(cm.getWinner() == "black_checkmate"&&game_on){
                cout << "Black won, Checkmate!" << endl;
                game_on = false;
                cm.clear_board();
                cm.addscore(true);
                if(server_active && multiplayer_mode){
                    multiplayer_mode = false;
                    break;
                }
            }else if(cm.getWinner() == "stalemate"&&game_on){
                cout << "Stalemate!" << endl;
                game_on = false;
                cm.addhalf();
                cm.clear_board();
                if(server_active && multiplayer_mode){
                    multiplayer_mode = false;
                    break;
                }
            }
            if(multiplayer_mode && server_active && game_on){
                if(Whose_move == net_self_colour){
                    try{
                        cout << "Please enter command: (move xx xx: to move a piece) (resign: to resign and quit the program)" << endl;
                        cin >> cmd;                     
                        if (cmd == "move"){
                            string pos1, pos2;
                            cin >> pos1 >> pos2;
                            string move_str = "move";
                            cm.movePiece(pos1, pos2);
                            string cppmsg =  move_str + " " + pos1 + " " + pos2;
                            char msgcstr[10];
                            strcpy(msgcstr,cppmsg.c_str());
                            if(host){
                                s.send_msg(msgcstr,10);
                            }else{
                                c.send_msg(msgcstr,10);
                            }
                        }else if(cmd == "resign"){
                            if(host){
                                s.send_msg("resign",6);
                            }else{
                                c.send_msg("resign",6);
                            }
                            cm.resign(net_self_colour);
                            if(!net_self_colour){
                                cout << "White has resigned, Black won." << endl;
                            }else{
                                cout << "Black has resigned, White won." << endl;
                            }
                            break;
                        }else{
                            cout << "Invalid command, please try again." << endl;
                            continue;
                        }
                    }catch(ChessException){
                        ce.print_exception();
                        cout << "Invalid command, please try again." << endl;
                        continue;
                    }
                }else{
                    string om;
                    cout << "Waiting for your opponent to move..." << endl;
                    if(host){   
                        om = s.read_msg(); // om stands for opponent's move
                    }else{
                        om = c.read_msg();
                    }
                    if (om.substr(0,4)=="move"){ // move w1 w2
                        string pos1, pos2;
                        pos1 = om.substr(5,2);
                        pos2 = om.substr(8,2);
                        cm.movePiece(pos1,pos2);
                    }else if(om.substr(0,6)=="resign"){ // resign    
                        cm.resign(!net_self_colour);
                        if(net_self_colour){
                            cout << "White has resigned, Black won." << endl;
                        }else{
                            cout << "Black has resigned, White won." << endl;
                        }
                        break;
                    }
                }
            }else{
                cout << "Please enter command: try -help for a list of commands" << endl;
            }
            if(game_on){
                if(cm.getWhoseMove()){
                    cout << "It is Black's turn." << endl;
                    if(multiplayer_mode){
                        continue;
                    }
                    if (black_comp) {
                        cm.compMove(cm.getWhoseMove());
                        continue;
                    }
                }else{
                    cout << "It is White's turn." << endl;
                    if(multiplayer_mode){
                        continue;
                    }
                    if (white_comp) {
                        cm.compMove(cm.getWhoseMove());
                        continue;
                    }
                }
            }
            if (loading) {
                cmd = load_queue.front();
                load_queue.pop();
                if (load_queue.empty()) loading = false;
                
            } else cin >> cmd;
            if (cmd == "game"){
                try{
                    string white_player, black_player;
                    if (loading) {
                        white_player = load_queue.front();
                        load_queue.pop();
                        black_player = load_queue.front();
                        load_queue.pop();
                        if (load_queue.empty()) loading = false;
                    } else cin >> white_player >> black_player;
                    if (white_player == "human" && black_player == "human") local_humans = true;
                    else local_humans = false;
                    if (white_player != "multiplayer" && white_player != "human" && white_player != "computer1" && white_player != "computer2" && white_player != "computer3" && white_player != "computer4"){
                        throw ce;
                    }
                    if (black_player != "multiplayer" && black_player != "human" && black_player != "computer1" && black_player != "computer2" && black_player != "computer3" && black_player != "computer4"){
                        throw ce;
                    }
                    if(game_on){
                        if(multiplayer_mode && server_active){
                            cout << "You are currently in an online game, you cannot start a new game." << endl;
                            cout << "Since you are in an online game, you need to restart the program in order to start a new game." << endl;
                            cout << "Resign your current game first, this will also quit your program." << endl;
                        }else{
                            cout << "You are currently in a game, Do you want to start a new game? y/n" << endl;
                            char yn;
                            cin >> yn;
                            while(true){
                                if (yn == 'n'){
                                    ce.continue_your_game_phrase = true;
                                    throw ce;
                                }else if(yn == 'y'){
                                    cm.re_init(white_player,black_player);
                                    if (white_player == "human" && black_player == "human") {
                                        savelog.open(savelog_name);
                                        savelog << "game human human" << endl;
                                        savelog.close();
                                    }
                                    if ((white_player == "computer1" || white_player == "computer2" || white_player == "computer3" || white_player == "computer4") &&
                                    (black_player == "computer1" || black_player == "computer2" || black_player == "computer3" || black_player == "computer4")) {
                                        count = 0;
                                    }
                                    break;
                                }else{
                                    cout << "Invalid command, please try again. y/n" << endl;
                                }
                            }
                        }
                    }else{
                        if(white_player == "multiplayer" && black_player == "multiplayer"){
                            char input;
                            std::cout << "If you want to host a game, input 0" << std::endl;
                            std::cout << "If you want to join a game, input 1" << std::endl;
                            std::cout << "Your input:" << std::endl;
                            std::cin >> input;
                            if (input == '0'){
                                try{
                                    string host_name;
                                    std::cout << "Please enter your name (no whitespace):" << std::endl;
                                    cin >> host_name;
                                    const char *playername = host_name.c_str();
                                    if(s.init((char*)playername)==1){
                                        multiplayer_mode = true;
                                        server_active = true;
                                        host = true;
                                    }
                                    while(true){
                                        std::cout << "If ready to play, input r" << std::endl;
                                        char ready;
                                        cin >> ready;
                                        if(ready == 'r'){
                                            s.send_msg("r",1);
                                            std::cout << "You are ready! Now waiting for the other player..." << std::endl;
                                            if(s.read_msg()=="r"){
                                                while(true){
                                                    std::cout << "You are the host, you get to pick the colors, which colour would you want to be? white/black" << std::endl;
                                                    std::string server_colour;
                                                    cin >> server_colour;
                                                    if(server_colour == "white"){
                                                        s.send_msg("white",5);
                                                        cm.init("multiplayer","multiplayer");
                                                        net_self_colour = false;
                                                        game_on = true;
                                                        break;
                                                    }else if(server_colour == "black"){
                                                        s.send_msg("black",5);
                                                        cm.init("multiplayer","multiplayer");
                                                        net_self_colour = true;
                                                        game_on = true;
                                                        break;
                                                    }else{
                                                        std::cout << "Invalid input, try again. white/black" << std::endl;
                                                    }
                                                }
                                                if(game_on){
                                                    break;
                                                }
                                            }
                                        }else{
                                            std::cout << "Invalid input, try again." << std::endl;
                                        }
                                    }
                                }catch(ChessException){
                                    ce.network = true;
                                    throw ce;
                                }
                            }else if(input == '1'){
                                string client_name;
                                std::cout << "Please enter your name (no whitespace):" << std::endl;
                                cin >> client_name;
                                const char *playername = client_name.c_str();
                                if(c.init((char*)playername)==1){
                                    multiplayer_mode = true;
                                    server_active = true;
                                }
                                while(true){
                                    std::cout << "If ready to play, input r" << std::endl;
                                    char ready;
                                    cin >> ready;
                                    if (ready == 'r'){
                                        c.send_msg("r",1);
                                        std::cout << "You are ready! Now waiting for the other player..." << std::endl;
                                        if(c.read_msg()=="r"){
                                            while(true){
                                                std::cout << "Waiting for the host to choose colour..." << std::endl;
                                                std::string response = c.read_msg();
                                                if(response == "white"){
                                                    cm.init("multiplayer","multiplayer");
                                                    cout << "The host chose white, you play as black." << endl;
                                                    net_self_colour = true;
                                                    game_on = true;
                                                    break;
                                                }else if(response == "black"){
                                                    cm.init("multiplayer","multiplayer");
                                                    cout << "The host chose black, you play as white." << endl;
                                                    net_self_colour = false;
                                                    game_on = true;
                                                    break;
                                                }else{
                                                    std::cout << "Invalid input, try again." << std::endl;
                                                }
                                            }
                                            if(game_on){
                                                break;
                                            }
                                        }
                                    }else{
                                        std::cout << "Invalid input, try again." << std::endl;
                                    }
                                }
                            }else{
                                ce.network = true;
                                throw ce;
                            }
                        }else{
                            if(!game_continuing){
                                cm.init(white_player,black_player);
                                game_continuing = true;
                            }else{
                                cm.re_init(white_player,black_player);
                                cm.clear_last_move();
                            }
                            multiplayer_mode = false;
                            game_on = true;
                            white_comp = (white_player != "human" && white_player != "multiplayer");
                            black_comp = (black_player != "human" && black_player != "multiplayer");
                            if (white_player == "human" && black_player == "human") {
                                savelog.open(savelog_name);
                                savelog << "game human human" << endl;
                                savelog.close();
                            }
                        }
                    }
                }catch(ChessException){
                    ce.print_exception();
                    cout << "Invalid command, please try again." << endl;
                }
            }else if (cmd == "move"){
                try{
                    if(!game_on){
                        ce.game_is_not_on = true;
                        throw ce;
                    }
                    string move_from, move_to;
                    if (loading) {
                        move_from = load_queue.front();
                        load_queue.pop();
                        move_to = load_queue.front();
                        load_queue.pop();
                        if (load_queue.empty()) loading = false;
                    } else cin >> move_from >> move_to;
                    cm.setrecordmove(move_from,move_to);
                    cm.movePiece(move_from, move_to);  
                    moves_so_far.push_back(string("move ") + move_from + string(" ") + move_to);
                }catch(ChessException){
                    cm.poprecordmove();
                    cout << "Invalid command, please try again." << endl;
                    ce.print_exception();
                }
            }else if(cmd == "reverse"){
                try{
                    if (local_humans) {
                        if(!game_on){
                            ce.game_is_not_on = true;
                            throw ce;
                        }
                        cm.re_init("human","human");
                        if(cm.getrecordmove().size() > 0){
                            for(int i = 0; i < cm.getrecordmove().size()-2;i+=2){
                                cm.movePiece(cm.getrecordmove()[i],cm.getrecordmove()[i+1]);
                            }
                            cm.poprecordmove();
                        }else{
                            throw ce;
                        }
                    } else {
                        cout << "Reverse can only be run on local human vs. human games." << endl;
                    }
                }catch(ChessException){
                    cout << "Invalid command, please try again." << endl;
                    ce.print_exception();
                }
            }else if(cmd == "resign"){
                try{
                    if(!game_on){
                        ce.game_is_not_on = true;
                        throw ce;
                    }
                    cm.resign(Whose_move);
                    if(cm.getWinner() == "black_resign"){ 
                        // Black won
                        cm.addscore(true);
                        cout << "White has resigned, Black won." << endl;
                    }else if (cm.getWinner() == "white_resign"){
                        // White won
                        cout << "Black has resigned, white won." << endl;
                        cm.addscore(false);
                    }
                    game_on = false;
                    cm.clear_board();
                    if (cm.getWhoseMove()) cm.switchWhoseMove();
                }catch(ChessException){
                    ce.print_exception();
                } 
            }else if(cmd == "setup"){    
                if(game_on){
                    cout << "A game is currently running" << endl;
                    continue;
                }else{
                    while(!game_on){ 
                        try{    // User may leave inputing quit and clearing the board.
                            cout << "Setup mode is on, please input your command:" << endl;
                            string input;
                            cin >> input;
                            if (input == "+"){
                                char piece;
                                string pos;
                                cin >> piece >> pos;
                                cm.setup('+',piece,pos,"");
                            }else if (input == "-"){
                                string pos;
                                cin >> pos;
                                cm.setup('-',' ',pos,"");
                            }else if (input == "="){
                                string colour;
                                cin >> colour;
                                cm.setup('=',' ',"",colour);
                            }else if (input == "done"){
                                if(cm.getSetupConditionsBool()){
                                    game_on = true;
                                    break;
                                }else{
                                    ce.setup = true;
                                    throw ce;
                                }
                            }else if(input == "quit"){
                                // Clear board
                                cm.clear_board();
                                //
                                break;
                            }else{
                                throw ce;
                            }
                        }catch(ChessException){
                            if(ce.setup){
                                ce.print_exception();
                            }else{
                                cout << "Invalid input" << endl;
                            }
                        }
                    }
            }
            }else if(cmd == "-help"){
                std::ifstream helptxt("help.txt");
                if(helptxt.is_open()){
                    std::cout << helptxt.rdbuf();
                }
                helptxt.close();
            } else if (cmd == "load") {
                if (!local_humans && game_on) {
                    cout << "Loading savelogs is only supported in local games between humans." << endl;
                } else if (game_on) {
                    cout << "You are currently in a game. Resign or finish the game to load a savelog." << endl;
                } else {
                    ifstream load;
                    load.open(savelog_name);
                    if (load) {
                        loading = true;
                        string x;
                        while (load >> x) load_queue.push(x);
                        // load into loading_buffer
                    } else {
                        cout << "There is no " << savelog_name << " in the current directory." << endl;
                        cout << "One will be generated for you on the completion of a local human vs. human game." << endl;
                    }
                }
            }
            
            else if(cmd == "quit"){
                break;
            }
        }
    }catch (ios::failure &){
        if(server_active){
            s.close_server();
            server_active = false;
            host = false;
        }
    }
    if(server_active){
        if(host){
            s.close_server();
        }
        server_active = false;
        host = false;
    }else{
        cm.print_score();
    }

    cout << "Thank you for playing, Goodbye!" << endl;

    if (local_humans) {
        savelog.open(savelog_name, ios::app);
        for (int i = 0; i < moves_so_far.size(); ++i) {
            savelog << moves_so_far.at(i) << endl;
        }
        savelog.close();
    }
    

    return 0;

}
