/*
Create a program that extracts data from google finance or yahoo finance.It gives the option of long / short / both.You pick the duration(from july 15 2013 to Present).
It the trades based on a slope of the moving average(which you set based on the number of days).
It shows profit and loss and trades made.

Step 1. Put the content of the file into an array (Complete)
-find a way to reverse display the arrays (Complete)
Step 2. Sort the array file into a structure
Step 3. Moving Average - and derivatives
step 4. ? ? ?
Step 5. Profit
*/

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <deque>
#include <thread> 
#include <future> 
#include <chrono>
#include <cmath>  

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <ncurses.h>
#include <time.h>




using namespace std;

int numberOfDays; //total number of days
int dayNumber = 0; //0 being the starting date

struct each_day {
	string date;
	float open;
	float high;
	float low;
	float close;
	float volume;
	float mov_avg[2];

};

each_day *day;


void float2structure(string date, float open, float high, float low, float close, float volume)
{
	if (dayNumber == 0)
	{
		day = new each_day[numberOfDays];
	}

	day[dayNumber].date = date;
	day[dayNumber].open = open;
	day[dayNumber].high = high;
	day[dayNumber].low = low;
	day[dayNumber].close = close;
	day[dayNumber].volume = volume;

	if (dayNumber != numberOfDays)
	{
		dayNumber++;
	}



}

void line2float(string line_string) //reads line and converts to float
{

	string date;
	float open;
	float high;
	float low;
	float close;
	float volume;

	string temp;

	int dataBlocks = 1;

	istringstream ss(line_string);
	while (!ss.eof())
	{
		switch (dataBlocks)
		{
		case 1:
			getline(ss, date, ',');
		case 2:
			getline(ss, temp, ',');
			if (temp == "-") {
				temp = "0";
			}
			open = stof(temp);
		case 3:
			getline(ss, temp, ',');
			if (temp == "-") {
				temp = "0";
			}
			high = stof(temp);
		case 4:
			getline(ss, temp, ',');
			if (temp == "-") {
				temp = "0";
			}
			low = stof(temp);
		case 5:
			getline(ss, temp, ',');
			if (temp == "-") {
				temp = "0";
			}
			close = stof(temp);
		case 6:
			getline(ss, temp, ',');
			if (temp == "-") {
				temp = "0";
			}
			volume = stof(temp);
		}

	}

	
	float2structure(date, open, high, low, close, volume);
}
void lineGet(ifstream &inFile) //gets line
{
	cout << "Reorganizing data... be patient..." << endl;

	vector<string> lines_in_reverse;
	string line;
	while (getline(inFile, line))
	{
		// Store the lines in reverse order.
		lines_in_reverse.insert(lines_in_reverse.begin(), line);

	}

	numberOfDays = lines_in_reverse.size() - 1;  // -1 removes date/open/close label

	for (int x = 0; x < numberOfDays; x++)
	{
		line2float(lines_in_reverse[x]);
	}


}

void SMA(int movingAvg, int mv_num)
{
	deque <float> sma;
	float smaSize = (float)movingAvg;

	float sum_of_elems = 0;

	for (int dayNumber = 0; dayNumber < numberOfDays; dayNumber++)
	{
		if (dayNumber <= smaSize - 1)
		{
			sma.push_front(day[dayNumber].close);
			day[dayNumber].mov_avg[mv_num] = 0;

			if (dayNumber == smaSize - 1)
			{
				for (float n : sma) {
					sum_of_elems += n;
				}

				day[dayNumber].mov_avg[mv_num] = sum_of_elems / smaSize;

				//	cout << day[dayNumber].moving_avg[mv_num] << endl;
			}


		}
		else
		{
			sum_of_elems = 0;
			sma.pop_back();
			sma.push_front(day[dayNumber].close);

			for (float n : sma)
			{
				sum_of_elems += n;
			}

			day[dayNumber].mov_avg[mv_num] = sum_of_elems / smaSize;

		}

	}

}

void decisionPoint_model(string startDate)
{
	dayNumber = 0;
	bool holdingLong = false;
	bool shortSell = false;
	float totalProfit = 0;
	float buyPrice;
	float sellPrice;
	float shortPrice;
	float coverPrice;

	//loop though each day, compare moving averages, buy and sell based on moving averages
	for (; dayNumber < numberOfDays; dayNumber++) {
		//cout << day[dayNumber].moving_avg[0] << " " << day[dayNumber].moving_avg[1] << endl;
		if (day[dayNumber].mov_avg[1] != 0) {

			if (day[dayNumber].mov_avg[0] < day[dayNumber].mov_avg[1] && holdingLong == true) {
				//sell
				sellPrice = day[dayNumber + 1].open;
				totalProfit += (sellPrice - buyPrice);
				cout << "Sell: $" << day[dayNumber + 1].open << " on " << day[dayNumber + 1].date << endl;
				cout << "Profit from trade: $" << sellPrice - buyPrice << endl;
				cout << "Total profit: $" << totalProfit << endl;
				cout << endl;
				holdingLong = false;
			}
			if (day[dayNumber].mov_avg[0] < day[dayNumber].mov_avg[1] && shortSell == false) {
				//short
				shortPrice = day[dayNumber + 1].open;
				cout << "Short: $" << day[dayNumber + 1].open << " on " << day[dayNumber + 1].date << ", ";
				shortSell = true;
			}
			if (day[dayNumber].mov_avg[0] > day[dayNumber].mov_avg[1] && shortSell == true) {
				//cover
				coverPrice = day[dayNumber + 1].open;
				totalProfit += (shortPrice - coverPrice);
				cout << "Cover: $" << day[dayNumber + 1].open << " on " << day[dayNumber + 1].date << endl;
				cout << "Profit from trade: $" << shortPrice - coverPrice << endl;
				cout << "Total profit: $" << totalProfit << endl;
				cout << endl;
				shortSell = false;
			}
			if (day[dayNumber].mov_avg[0] > day[dayNumber].mov_avg[1] && holdingLong == false) {
				//buy
				buyPrice = day[dayNumber + 1].open;
				cout << "Buy: $" << day[dayNumber + 1].open << " on " << day[dayNumber + 1].date << ", ";
				holdingLong = true;
			}
		}

	}
	cout << endl;
	cout << "Total profits from strategy: $" << totalProfit << endl;
	cout << endl;
}

void decisionPoint_model_call() {

	cout << "\nWe are going to perfrom a decision point analysis." << endl;
	cout << "This requires two moving averages relatively close by (eg. 10 and 15 day moving averages)." << endl;
	cout << endl;
	int movingavg;
	cout << "Set the first moving average (ex. 10):";
	cin >> movingavg;
	SMA(movingavg, 0);

	cout << endl;
	cout << "Set the second moving average (ex. 15):";
	cin >> movingavg;
	SMA(movingavg, 1);


	cout << "\nData stretches from: " << day[0].date << " to " << day[numberOfDays - 1].date << endl;
	
	string startDate;
	cout << "Enter a date to begin analysis: ";
	cin >> startDate;
	cout << endl;

	decisionPoint_model(startDate);
	cout << "Open price on first day (given data): $" << day[0].open << endl;
	cout << "Close price on last day (given data): $" << day[numberOfDays - 1].close << endl;
	cout << "For a total price change of: $" << day[numberOfDays - 1].close - day[0].open << endl;
	cout << endl;

}
void file_get() {


functionStart:
	string filename;
	cout << "\nEnter file name (ex. goog.csv):";
	cin >> filename;

	ifstream inFile(filename.c_str());
	if (inFile.is_open()) {
		cout << "\nLoading file...\n" << endl;
		lineGet(inFile);
		inFile.close();
	}
	else {
		cout << "No file was found on disk, retrieving one from the web..." << endl;
		system("python stockFetch.py");
		goto functionStart;
	}
}



int kbhit(void)
{
    int ch = getch();
    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}

struct sdata {
	char timeStamp[25];
	char price[25];
	char changeprice[25];
	char changepercent[25];
	sem_t mutex;

	float buy_position;
	float buy_avgprice;
	float total_buy_price;

	float sell_position;
	float sell_avgprice;
	float total_sell_price;

	float profit;
}*data;





bool finished = true; 
void script(string exec){
	system(exec.c_str()); //"START /B" 
	finished = true;
}

void swing_trader_live() {
	//get data
	//store and process data
	//execute
	int shmid;
	key_t key = 101;

	size_t shmsize = sizeof(struct sdata);
	shmid = shmget(key, shmsize, IPC_CREAT | 0777);
	data = (sdata *)shmat(shmid, 0, 0);

	sem_init(&(data->mutex), 1, 0);

	
	string stockName;
	string exchange;

	cout << "Enter stock name: ";
	cin >> stockName;
	cout << "Enter name of exchange: ";
	cin >> exchange;
	string execute = "python liveGet.py " + stockName + " " + exchange;

    cout << "\nFamiliarize yourself with the following commands:"<<endl;
    cout << "buy 10 100" <<endl;
    cout << "sell 10 110.25" <<endl;
    cout << "Leave the price field blank if you want to trade at the market price."<<endl;
    sleep(10);
    initscr();
  cbreak();
    noecho();
   nodelay(stdscr, TRUE);
    //scrollok(stdscr, TRUE);

		int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

string lastPrice;
//time_t start, end;
//double elapsed;
//int startTime = 0;
string order;


	int y;
	int x;

bool firstLetter = true;

	
	

	while (true) {

			if (finished){
				finished = false;
				thread (script, execute).detach();

			}
	      
	        	        	


	    string currPrice(data->price);
		string currTime(data->timeStamp);
		string changepercent(data->changepercent);
		string changeprice(data->changeprice);

	//  if (currPrice != lastPrice) {

		mvprintw(0, maxX-45, "%s -- %s  	%s",stockName.c_str(),currTime.c_str(),currPrice.c_str());
		mvprintw(1, maxX-16, "%s (%s\%)",changeprice.c_str(),changepercent.c_str());

		lastPrice = currPrice;
	  //}

		if (firstLetter){
			if (order.empty()){
				move(1,0);
				clrtoeol();
			}
			mvprintw(1,0,">>");

			move(1,3);
			y = 1;
			x = 3;
			firstLetter = false;
		}else{
			move(y,x);
				
		}
		
		refresh();

	    if( kbhit()){

	    	char input = getch();

	     	if ((int)input != 127){
			    order += (char)input;
			}
			else{
			    if (!order.empty()){
			        order.pop_back();
				    printw("\b");
			        printw(" ");
			        printw("\b");
			        			        		
			    }

			}
		
			if (input == 10){ //what happen when you press enter
			////////////////////////////////////////////////////////
			//parse string
			// if legal 
			//
	int dataBlocks = 1;
	string temp;
	vector<string> order_vector(3);
	istringstream ss(order);
	
	try
	{
	   // protected code

		while (!ss.eof())
		{
			switch (dataBlocks)
			{
		    case 0:
		    	throw 1;
			
			case 1:
				getline(ss, temp, ' ');
				order_vector[0] = temp;

			case 2:
			    getline(ss, temp, ' ');
			    order_vector[1] = temp;
				    if (!stof(temp)){
				    	dataBlocks = 0;
				    }
				    if(temp[temp.length()-1] == '\n'){
				    	order_vector[2] = "";
				    	break;
					}

			case 3:
				getline(ss, temp, ' ');
				order_vector[2] = temp;
				    if (!stof(temp)){
				    	dataBlocks = 0;
				    }
			default:
				dataBlocks = 0;
				


			}


		}
	}catch(...)
	{
		  // code to handle any exception
		order_vector[0] = "INVALID COMMAND";
		order_vector[1].clear();
		order_vector[2].clear();
	}

	if (order_vector[0] == "buy"){
		if (order_vector[2].empty()){
			thread (script, execute).join();
			string currPrice(data->price);
			string commaCheck = ",";
			string::size_type i = currPrice.find(commaCheck);
			if (i != std::string::npos){
				currPrice.erase(i, commaCheck.length());
			}
  				

			data->buy_position += stof(order_vector[1]);

			float currtransaction_buy;
			currtransaction_buy = stof(currPrice)*stof(order_vector[1]);

			data->total_buy_price  += currtransaction_buy;

			data->buy_avgprice = data->total_buy_price / data->buy_position;


			move(5,0);
	  		clrtoeol();
			mvprintw(5,0, "Current long positions: %.0f -- Avg. price of $%.2f", data->buy_position , data->buy_avgprice);
			move(6,0);
	  		clrtoeol();



		}
		if (!order_vector[2].empty()){


		}

	}
	if (order_vector[0] == "sell"){
		if (order_vector[2].empty()){
			thread (script, execute).join();
			string currPrice(data->price);
			string commaCheck = ",";
			string::size_type i = currPrice.find(commaCheck);
			if (i != std::string::npos){
				currPrice.erase(i, commaCheck.length());
			}
			float changeshare = data->buy_position;

			if (stof(order_vector[1]) > data->buy_position ){
				data->buy_position  = 0;
			}else{
				data->buy_position -= stof(order_vector[1]);
			}

			float currtransaction_sell;
			currtransaction_sell = stof(currPrice)*stof(order_vector[1]);

			if (currtransaction_sell > data->total_buy_price ){
				data->total_buy_price = 0;
			}else{
				data->total_buy_price  -= currtransaction_sell;
			}

			data->profit += (stof(currPrice) - data->buy_avgprice )*(changeshare - data->buy_position);

				if (data->buy_position < 1 ){
					data->buy_avgprice = 0;
				}
	  		move(5,0);
	  		clrtoeol();
			mvprintw(5,0, "Current long positions: %.0f -- Avg. price of $%.2f", data->buy_position , data->buy_avgprice);
	  		move(6,0);
	  		clrtoeol();
	  		mvprintw(6,0, "Sold %.0f at $%.2f", changeshare - data->buy_position , stof(currPrice));

	  		move(11,0);
	  		clrtoeol();
	  		mvprintw(11,0, "Total profit: $%.2f", data->profit);

		}
		if (!order_vector[2].empty()){

			
		}
		
	}
	if (order_vector[0] == "short"){
		if (order_vector[2].empty()){
			thread (script, execute).join();
			string currPrice(data->price);
			string commaCheck = ",";
			string::size_type i = currPrice.find(commaCheck);
			if (i != std::string::npos){
				currPrice.erase(i, commaCheck.length());
			}

			data->sell_position += stof(order_vector[1]);

			float currtransaction_short;
			currtransaction_short = stof(currPrice)*stof(order_vector[1]);

			data->total_sell_price  += currtransaction_short;

			data->sell_avgprice = data->total_sell_price / data->sell_position;

			move(8,0);
	  		clrtoeol();
			mvprintw(8,0, "Current short positions: %.0f -- Avg. price of $%.2f", data->sell_position , data->sell_avgprice);
			move(9,0);
	  		clrtoeol();

		}
		if (!order_vector[2].empty()){

			
		}
		
	}
	if (order_vector[0] == "cover"){
		if (order_vector[2].empty()){
			thread (script, execute).join();
			string currPrice(data->price);
			string commaCheck = ",";
			string::size_type i = currPrice.find(commaCheck);
			if (i != std::string::npos){
				currPrice.erase(i, commaCheck.length());
			}
			float changeshare = data->sell_position;

			if (stof(order_vector[1]) > data->sell_position ){
				data->sell_position  = 0;
			}else{
				data->sell_position -= stof(order_vector[1]);
			}

			float currtransaction_cover;
			currtransaction_cover = stof(currPrice)*stof(order_vector[1]);

			if (currtransaction_cover < data->total_sell_price ){
				data->total_sell_price = 0;
			}else{
				data->total_sell_price  -= currtransaction_cover;
			}

			data->profit += (data->sell_avgprice - stof(currPrice)  )*(changeshare - data->sell_position);

							if (data->sell_position < 1 ){
					data->sell_avgprice = 0;
				}

	  		move(8,0);
	  		clrtoeol();
			mvprintw(8,0, "Current short positions: %.0f -- Avg. price of $%.2f", data->sell_position , data->sell_avgprice);
	  		move(9,0);
	  		clrtoeol();
	  		mvprintw(9,0, "Sold %.0f at $%.2f", changeshare - data->buy_position , stof(currPrice));

	  		move(11,0);
	  		clrtoeol();
	  		mvprintw(11,0, "Total profit: $%.2f", data->profit);

		}
		if (!order_vector[2].empty()){

			
		}
		
	}
	if (order_vector[0] == "exit"){
		endwin();
		break;
	}

		    ////////////////////////////////////////////////////////
				move(3,0);
		        printw("Order: ");
		        printw("%s %s %s",order_vector[0].c_str(), order_vector[1].c_str(),order_vector[2].c_str());
				order.clear();
				firstLetter = true;
	
		    }
		    else{ 
	            	
		        if ( (int) input != 127){
					printw("%c",input);
		        }
			}
		    
		    refresh();
		    getyx(stdscr, y, x);

	       }


	}

	// Detach and remove shared memory
	shmdt((void *)shmid);
	shmctl(shmid, IPC_RMID, NULL);


}


int main()
{

	cout << endl;
	cout << "What type on analysis would you like to perform?" << endl;
	cout << "1. Decision Point Analysis - based on historical data." << endl;
	cout << "2. Swing trading program - live trading." << endl;
	cout << "3. Live algorithmic trading." << endl;
	cout << endl;
	cout << "Enter number: ";
	int analysisNumber;
	cin >> analysisNumber;

	switch (analysisNumber) {
	case 1:
		file_get();
		decisionPoint_model_call();

		break;

	case 2:
		swing_trader_live();
		break;
	
	//case 3:


	}

	cin.ignore();
	return 0;


}


