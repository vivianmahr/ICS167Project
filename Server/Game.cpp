#include "Game.h"

using namespace std;

Game::Game(int w, int h){
	SCREEN_WIDTH = w;
	SCREEN_HEIGHT = h;
	pointsRight = 0;
	pointsLeft = 0;
	_ball = new Ball(SCREEN_WIDTH, SCREEN_HEIGHT);
	_paddle_left = new Paddle(true, SCREEN_WIDTH, SCREEN_HEIGHT);
	_paddle_right = new Paddle(false, SCREEN_WIDTH, SCREEN_HEIGHT);
	g_state = Game::Waiting;
	paddle1Moving = false;
	paddle2Moving = false;
}
Game::~Game(){

}

void Game::paddlePosition(int playerId, std::string input){
	string posx = "", posy = "";
	string space = " ";
	if (input.substr(0,16) == "paddlePosition: "){
		input.erase(0, 16);
		for (int i = 0; i < input.size(); i++){
			if (input[i] != space[0]){
				posx += input[i];
			}
			else{
				input.erase(0, i);
				i = input.size()+1;
			}
		}
		for (int j = 0; j < input.size(); j++){
				posy += input[j];
		}
		int x = stoi(posx);
		int y = stoi(posy);
		if (x>(SCREEN_WIDTH / 2))
			_paddle_right->setPosition(x, y);
		else
			_paddle_left->setPosition(x, y);
		
	}
}

void Game::setOffset(int off){
	server_offset = off;
}

void Game::savePlayerID(int playerId, std::string id){
	if (id.substr(0, 3) == "id:"){
		std::string strPlayer;
		if (playerId == 0)
			strPlayer = PlayerOneID = id.substr(3, id.size());
		else if (playerId == 1)
			strPlayer = PlayerTwoID = id.substr(3, id.size());
		cout << strPlayer + " has connected to the server." << endl;
	}
}

void Game::CalculateLatency(int playerId, std::string ClientMessageTimes){
	int latency = 0;
	
	ClientMessageTimes.erase(0, 5);
	TimeFormat TimeReceived = CreateTimeFormat(ClientMessageTimes);
	
	ClientMessageTimes.erase(0, 5);
	TimeFormat TimeSent = CreateTimeFormat(ClientMessageTimes);
	
	latency = ((TimeReceived.hour - TimeSent.hour) * 600000) + ((TimeReceived.min - TimeSent.min) * 60000) + ((TimeReceived.sec - TimeSent.sec) * 1000) + (TimeReceived.millisec - TimeSent.millisec);
	latency -= 33; //33 is the fixed delay

	cout << "Latency (client " << playerId << "):" << latency << " ms" << endl;
}

Game::TimeFormat Game::CreateTimeFormat(std::string& msg){
	TimeFormat newt;
	std::string next_part = ":";
	std::string next_time = "T";
	std::vector<std::string> new_vector;
	std::string new_str;
	int pos = 0;
	for (int i = 0; i < msg.length(); i++){
		if (msg[i] != next_time[0]){
			if (msg[i] == next_part[0]){
				int x = i - pos;
				new_str = msg.substr(pos, x);
				pos = i;
				if (new_str.length() > 0)
					new_vector.push_back(new_str);
				msg.erase(msg.begin() + i);
			}
		}
		else{
			msg.erase(0, i);
			i = 200; //to break out of for
		}
		
	}
	newt.hour = atoi(new_vector[0].c_str());
	newt.min = atoi(new_vector[1].c_str());
	newt.sec = atoi(new_vector[2].c_str());
	newt.millisec = atoi(new_vector[3].c_str());
	return newt;
}

void Game::Update(){
	_ball->Move();
	_ball->CheckCollision(_paddle_right);
	_ball->CheckCollision(_paddle_left);
	if (_ball->CheckLimits()){
		if (_ball->getLastDirection() == _ball->LEFT)
			pointsRight += 1;
		else if (_ball->getLastDirection() == _ball->RIGHT)
			pointsLeft += 1;
	}
	if (pointsRight >= 10 || pointsLeft >= 10){
		pointsRight = 0;
		pointsLeft = 0;
		_ball->Respawn();
	}
}

int Game::getHeight(){
	return SCREEN_HEIGHT;
}
int Game::getWidth(){
	return SCREEN_WIDTH;
}


Ball* Game::getBall(){
	return _ball;
}



int Game::getLeftPoints(){
	return pointsLeft;
}
int Game::getRightPoints(){
	return pointsRight;
}

std::string Game::getPlayerOneID(){
	return PlayerOneID;
}
std::string Game::getPlayerTwoID(){
	return PlayerTwoID;
}

Game::GameState Game::getState(){
	return g_state;
}

Paddle* Game::getRightPaddle(){
	g_state = Game::Playing;
	return _paddle_right = new Paddle(false, SCREEN_WIDTH, SCREEN_HEIGHT);
}

Paddle* Game::getLeftPaddle(){
	return _paddle_left;
}

void Game::PlayerDisconnected(){
	g_state = Game::Disconnected;
}