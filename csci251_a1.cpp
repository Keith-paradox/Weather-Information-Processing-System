#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <string.h>
#include <fstream>
#include <algorithm>
#include <iomanip>

using namespace std;

//for reading files
string filename, data;

//hit enter to go back to main menu
string goBack;

//check if all file is read or not
bool read_allFiles = false;

//checking cloud cover file is added
bool cloudCover_added = false;

//to display city map
bool city_display = false;

//checking if config file is read
bool config_read = false;

//chek if gridX exist
bool gridX_found = false;

//check if gridY exist
bool gridY_found = false;

//checking if there is 3 files written in config
int file_found = 0;
bool displayLine = false;

//needs for split function
char ch;
vector<string> strings;

//main menu option
int userChoice;

//necessary for drawing grid
int gridX, gridY, userXinput, userYinput;
int gridXsign,gridX_start,gridX_end,gridYsign,gridY_start,gridY_end;
string gridX_input, gridY_input;

//declaring functions
void drawGrid(int gridX_start, int gridX_end, int gridY_start, int gridY_end ,vector<int> vect_a, vector<int> vect_b, vector<int> c, vector<string> d);
bool readFile(string filename);
void split(string str, char seperator, vector<string> datas);
void readConfig();
void startMenu();
string cal_LMH(int value);
void summary();
float round(float n);
string rain_prob(string cloud, string pressure);

//implementations
bool showReport = true;
vector<int> surrounding_x;
vector<int> surrounding_y;
void implementation(int gridX_start, int gridX_end, int gridY_start, int gridY_end ,vector<int> vect_a, vector<int> vect_b, vector<int> vect_c, vector<int> vect_d, vector<string> e);

//vector arrays for each data
vector<int> cityLocation_x;
vector<int> cityLocation_y;
vector<int> cloudCover_x;
vector<int> cloudCover_y;
vector<int> pressure_x;
vector<int> pressure_y;
vector<int> cityId;
vector<string> cityName;
vector<int> cloudCover_content;
vector<string> cloudCover_LMH;
vector<int> pressure_content;
vector<string> pressure_LMH;
vector<int> dummy_int;
vector<string> dummy_str;

int main()
{	
	startMenu();
	return 0;
}

//to round out the number
float round(float n){
	float result = (int)(n * 100 + .5);
	return (float)result/100;
}


//reading config file
void readConfig()
{	
	displayLine = false;
	gridX_found = false;
	gridY_found = false;
	read_allFiles = false;
	file_found = 0;
	vector<string> dataItems;
	ifstream file;
	
	//asking user to type in the config file
	cout << "Please enter config filename: ";
	cin >> filename;
	
	file.open(filename);
	
	if(file.good())
	{
		
		while(! file.eof()){
			//reading each line from config.txt
			getline(file, data);
			
			//finding grid x range in config
			if(data.find("GridX") != string::npos){
				int sign = data.find("=");
				
				//taking only the range numbers
				gridX_input = data.substr(sign+1);
				cout << "\nReading in GridX_IdxRange: " << gridX_input << "...done!" << endl;
				
				gridX_found = true;
				
			}
			//finding grid y range in config
			else if(data.find("GridY") != string::npos){
				int sign = data.find("=");
				gridY_input = data.substr(sign + 1);
				cout << "Reading in GridY_IdxRange: " << gridY_input << "...done!" << endl;
				gridY_found = true;
			}
			//finding the files(.txt) in config
			else if(data.find(".txt") != string::npos){
				if(displayLine == false){
					cout << "\nStoring data from input file: " << endl;
					displayLine = true;
				}
				
				//calling the function that reads the files and save the datas
				//if file is read successfully will display done and if not then failed
				cout << data;
				if(readFile(data)){
					cout << "...done!" << endl;
				}
				else{
					cout << "...failed!" << endl;
				}
				
				
			}
		}
		
		if(gridX_found == true and gridY_found == true){
			gridXsign = gridX_input.find("-");
			
			//taking the starting coordinate of gridX
			string gridX_start_str = gridX_input.substr(0, gridXsign);
			gridX_start = stoi(gridX_start_str);
			
			//taking the ending coordinate of gridX
			string gridX_end_str = gridX_input.substr(gridXsign + 1);
			gridX_end = stoi(gridX_end_str);
			
			gridYsign = gridY_input.find("-");
			
			//taking the starting coordinate of gridY
			string gridY_start_str = gridY_input.substr(0, gridYsign);
			gridY_start = stoi(gridY_start_str);
			
			//taking the ending coordinate of gridY
			string gridY_end_str = gridY_input.substr(gridYsign + 1);
			gridY_end = stoi(gridY_end_str);
		}
		
		//if the gridX or gridY range is not defined in config
		else{
			if(gridX_found == false and gridY_found == false){
				cout << "\nGridX_IdxRange and GridX_IdxRange can't be found." << endl;
			}
			else if(gridX_found == false){
				cout << "\nGridX_IdxRange can't be found." << endl;
			}
			else{
				cout << "\nGridY_IdxRange can't be found." << endl;
			}
			
		}
		
		//if all three files are found
		if(file_found == 3 and gridX_found == true and gridY_found == true){
			cout << "\nAll records successfully stored. Going back to main menu..." << endl;
			read_allFiles = true;
		}
		else{
			cout << "\nSome records are missing, try another file. Going back to main menu..." << endl;
		}
	
	}
	else
	{
		cout << "Invalid filename." << endl;
	}
	
	file.close();
	
}


//weather forecast summary report
void summary()
{
	//stores all of the cities in these vectors without duplicates
	vector<string> cities;
	vector<int> cities_ids;
	
	//stores all the surrouding areas of the cities without duplicates
	vector<string> cloud_surrounding_areas;
	vector<string> pressure_surrounding_areas;
	
	//store all the values of cities and surrounding areas for calculation
	vector<float> total_cloudCover;
	vector<float> total_pressure;
	
	//stores the total amount of cities to get average
	vector<float> total_cities_cloud;
	vector<float> total_cities_pressure;
	
	//reading all the cities from file and eliminating the duplicates
	for(int i = 0; i < cityName.size(); i++){
		if(count(cities.begin(), cities.end(), cityName[i])){
			continue;
		}else{
			//creating an index for each city
			cities.push_back(cityName[i]);
			cities_ids.push_back(cityId[i]);
			
			total_cloudCover.push_back(0);
			total_cities_cloud.push_back(0);

			total_pressure.push_back(0);
			total_cities_pressure.push_back(0);
		}
	}
	
	//searching the coordinates of each city and comparing it with the coordinates of cloudCover
	for(int i = 0; i < cityLocation_x.size(); i++){
		for(int j = 0; j < cloudCover_x.size(); j++){
			if(cityLocation_x[i] == cloudCover_x[j] and cityLocation_y[i] == cloudCover_y[j]){
			
				//to store the values in exact index that the cities are assigned
				for(int k = 0; k < cities.size(); k++){
					if(cityName[i] == cities[k]){
						//for searching the surrouding area of a city
						for(int l = 0; l < cloudCover_x.size(); l++){
							
							//if that area is already added then skip
							if(count(cloud_surrounding_areas.begin(), cloud_surrounding_areas.end(), to_string(cloudCover_x[l])+","+to_string(cloudCover_y[l])+"-"+cities[k])){
								continue;	
							}
							//if it is not added then add it to the vector and also its value to the total
							else{
							
							//the city area
							if(cloudCover_x[l] == cloudCover_x[j] and cloudCover_y[l] == cloudCover_y[j]){
								cloud_surrounding_areas.push_back(to_string(cloudCover_x[l])+","+to_string(cloudCover_y[l])+"-"+cities[k]);
								total_cloudCover[k] += cloudCover_content[l];
								total_cities_cloud[k]++;
							}
							
							//left area of the city
							else if(cloudCover_x[l] == cloudCover_x[j]-1 and cloudCover_y[l] == cloudCover_y[j]){
								cloud_surrounding_areas.push_back(to_string(cloudCover_x[l])+","+to_string(cloudCover_y[l])+"-"+cities[k]);
								total_cloudCover[k] += cloudCover_content[l];
								total_cities_cloud[k]++;
								surrounding_x.push_back(cloudCover_x[l]);
								surrounding_y.push_back(cloudCover_y[l]);
							}
							
							//right area of the city
							else if(cloudCover_x[l] == cloudCover_x[j]+1 and cloudCover_y[l] == cloudCover_y[j]){
								cloud_surrounding_areas.push_back(to_string(cloudCover_x[l])+","+to_string(cloudCover_y[l])+"-"+cities[k]);
								total_cloudCover[k] += cloudCover_content[l];
								total_cities_cloud[k]++;
								surrounding_x.push_back(cloudCover_x[l]);
								surrounding_y.push_back(cloudCover_y[l]);
							}
							
							//bottom area of the city
							else if(cloudCover_x[l] == cloudCover_x[j] and cloudCover_y[l] == cloudCover_y[j]-1){
								cloud_surrounding_areas.push_back(to_string(cloudCover_x[l])+","+to_string(cloudCover_y[l])+"-"+cities[k]);
								total_cloudCover[k] += cloudCover_content[l];
								total_cities_cloud[k]++;
								surrounding_x.push_back(cloudCover_x[l]);
								surrounding_y.push_back(cloudCover_y[l]);
							}
							
							//top area of the city
							else if(cloudCover_x[l] == cloudCover_x[j] and cloudCover_y[l] == cloudCover_y[j]+1){
								cloud_surrounding_areas.push_back(to_string(cloudCover_x[l])+","+to_string(cloudCover_y[l])+"-"+cities[k]);
								total_cloudCover[k] += cloudCover_content[l];
								total_cities_cloud[k]++;
								surrounding_x.push_back(cloudCover_x[l]);
								surrounding_y.push_back(cloudCover_y[l]);
							}
							
							//bottom left area of the city
							else if(cloudCover_x[l] == cloudCover_x[j]-1 and cloudCover_y[l] == cloudCover_y[j]-1){
								cloud_surrounding_areas.push_back(to_string(cloudCover_x[l])+","+to_string(cloudCover_y[l])+"-"+cities[k]);
								total_cloudCover[k] += cloudCover_content[l];
								total_cities_cloud[k]++;
								surrounding_x.push_back(cloudCover_x[l]);
								surrounding_y.push_back(cloudCover_y[l]);
							}
							
							//top left area of the city
							else if(cloudCover_x[l] == cloudCover_x[j]-1 and cloudCover_y[l] == cloudCover_y[j]+1){
								cloud_surrounding_areas.push_back(to_string(cloudCover_x[l])+","+to_string(cloudCover_y[l])+"-"+cities[k]);
								total_cloudCover[k] += cloudCover_content[l];
								total_cities_cloud[k]++;
								surrounding_x.push_back(cloudCover_x[l]);
								surrounding_y.push_back(cloudCover_y[l]);
							}
							
							//bottom right of the city
							else if(cloudCover_x[l] == cloudCover_x[j]+1 and cloudCover_y[l] == cloudCover_y[j]-1){
								cloud_surrounding_areas.push_back(to_string(cloudCover_x[l])+","+to_string(cloudCover_y[l])+"-"+cities[k]);
								total_cloudCover[k] += cloudCover_content[l];
								total_cities_cloud[k]++;
								surrounding_x.push_back(cloudCover_x[l]);
								surrounding_y.push_back(cloudCover_y[l]);
							}
							
							//top right area of the city
							else if(cloudCover_x[l] == cloudCover_x[j]+1 and cloudCover_y[l] == cloudCover_y[j]+1){
								cloud_surrounding_areas.push_back(to_string(cloudCover_x[l])+","+to_string(cloudCover_y[l])+"-"+cities[k]);
								total_cloudCover[k] += cloudCover_content[l];
								total_cities_cloud[k]++;
								surrounding_x.push_back(cloudCover_x[l]);
								surrounding_y.push_back(cloudCover_y[l]);
							}
							
							}
						}
						
						
					}
				}
			}
			
		}
		
		//this is for pressure and the same as cloudCover
		for(int j = 0; j < cloudCover_x.size(); j++){
			if(cityLocation_x[i] == pressure_x[j] and cityLocation_y[i] == pressure_y[j]){
				for(int k = 0; k < cities.size(); k++){
					if(cityName[i] == cities[k]){
					for(int l = 0; l < pressure_x.size(); l++){
						
							if(count(pressure_surrounding_areas.begin(), pressure_surrounding_areas.end(), to_string(pressure_x[l])+","+to_string(pressure_y[l])+"-"+cities[k])){
								continue;	
							}
							else{
							if(pressure_x[l] == pressure_x[j] and pressure_y[l] == pressure_y[j]){
								pressure_surrounding_areas.push_back(to_string(pressure_x[l])+","+to_string(pressure_y[l])+"-"+cities[k]);
								total_pressure[k] += pressure_content[l];
								total_cities_pressure[k]++;
							}
							else if(pressure_x[l] == pressure_x[j]-1 and pressure_y[l] == pressure_y[j]){
								pressure_surrounding_areas.push_back(to_string(pressure_x[l])+","+to_string(pressure_y[l])+"-"+cities[k]);
								total_pressure[k] += pressure_content[l];
								total_cities_pressure[k]++;
							}
							else if(pressure_x[l] == pressure_x[j]+1 and pressure_y[l] == pressure_y[j]){
								pressure_surrounding_areas.push_back(to_string(pressure_x[l])+","+to_string(pressure_y[l])+"-"+cities[k]);
								total_pressure[k] += pressure_content[l];
								total_cities_pressure[k]++;
							}
							else if(pressure_x[l] == pressure_x[j] and pressure_y[l] == pressure_y[j]-1){
								pressure_surrounding_areas.push_back(to_string(pressure_x[l])+","+to_string(pressure_y[l])+"-"+cities[k]);
								total_pressure[k] += pressure_content[l];
								total_cities_pressure[k]++;
							}
							else if(pressure_x[l] == pressure_x[j] and pressure_y[l] == pressure_y[j]+1){
								pressure_surrounding_areas.push_back(to_string(pressure_x[l])+","+to_string(pressure_y[l])+"-"+cities[k]);
								total_pressure[k] += pressure_content[l];
								total_cities_pressure[k]++;
							}
							else if(pressure_x[l] == pressure_x[j]-1 and pressure_y[l] == pressure_y[j]-1){
								pressure_surrounding_areas.push_back(to_string(pressure_x[l])+","+to_string(pressure_y[l])+"-"+cities[k]);
								total_pressure[k] += pressure_content[l];
								total_cities_pressure[k]++;
							}
							else if(pressure_x[l] == pressure_x[j]-1 and pressure_y[l] == pressure_y[j]+1){
								pressure_surrounding_areas.push_back(to_string(pressure_x[l])+","+to_string(pressure_y[l])+"-"+cities[k]);
								total_pressure[k] += pressure_content[l];
								total_cities_pressure[k]++;
							}
							else if(pressure_x[l] == pressure_x[j]+1 and pressure_y[l] == pressure_y[j]-1){
								pressure_surrounding_areas.push_back(to_string(pressure_x[l])+","+to_string(pressure_y[l])+"-"+cities[k]);
								total_pressure[k] += pressure_content[l];
								total_cities_pressure[k]++;
							}
							else if(pressure_x[l] == pressure_x[j]+1 and pressure_y[l] == pressure_y[j]+1){
								pressure_surrounding_areas.push_back(to_string(pressure_x[l])+","+to_string(pressure_y[l])+"-"+cities[k]);
								total_pressure[k] += pressure_content[l];
								total_cities_pressure[k]++;
							}
							
							}
						}
					}
					}
				
			}
		}
		
		
	}
	
	if(showReport == true){
		//calculating and displaying each city and its data
		for(int i = 0; i < cities.size(); i++){
			cout << endl;
			
			float ave_cloud = 0;
			float ave_pressure = 0;
			
			ave_cloud = total_cloudCover[i] / total_cities_cloud[i];
			
			ave_pressure = total_pressure[i] / total_cities_pressure[i];
			
			cout << "City Name\t: " << cities[i] << endl;
			cout << "City ID\t\t: " << cities_ids[i] << endl;
			cout << "Ave. Cloud Cover\t(ACC)\t: " << setprecision(2) << fixed << round(ave_cloud) << " (" << cal_LMH(ave_cloud) << ")" << endl;
			cout << "Ave. Pressure\t\t(AP)\t: " << setprecision(2) << fixed << round(ave_pressure) << " (" << cal_LMH(ave_pressure) << ")" << endl;
			cout << "Probability of Rain\t(%)\t: " << rain_prob(cal_LMH(ave_pressure), cal_LMH(ave_cloud)) << endl;
			cout << endl;
		}
	}
	
	
	
}

//calculating rain probability
string rain_prob(string a, string b){
	string str = "";
	
	if(a == "L" and b == "H"){
		str = "90.00\n~~~~\n~~~~~\n\\\\\\\\\\";
	}
	else if(a == "L" and b == "M"){
		str = "80.00\n~~~~\n~~~~~\n \\\\\\\\";
	}
	else if(a == "L" and b == "L"){
		str = "70.00\n~~~~\n~~~~~\n  \\\\\\";
	}
	else if(a == "M" and b == "H"){
		str = "60.00\n~~~~\n~~~~~\n   \\\\";
	}
	else if(a == "M" and b == "M"){
		str = "50.00\n~~~~\n~~~~~\n    \\";
	}
	else if(a == "M" and b == "L"){
		str = "40.00\n~~~~\n~~~~~";
	}
	else if(a == "H" and b == "H"){
		str = "30.00\n~~~\n~~~~";
	}
	else if(a == "H" and b == "M"){
		str = "20.00\n~~\n~~~";
	}
	else if(a == "H" and b == "L"){
		str = "10.00\n~\n~~";
	}
	
	return str;
}

//calculating LMH
string cal_LMH(int value)
{
	string symbol = "NULL";
	
	if(0 <= value and value < 35){
		symbol = "L";
	}
	else if(35 <= value and value < 65){
		symbol = "M";
	}
	else if(65 <= value and value < 100){
		symbol = "H";
	}
	
	return symbol;
}

//Main menu
void startMenu(){
	bool quit = false;
	
	//loop to keep asking user until he quit
	while(!quit){
		cout << "StudentID\t: 8039173" << endl;
		cout << "StudentName\t: Min Khant Thu" << endl;
		cout << "-----------------------------------------------" << endl;
		
		cout << "Welcome to Weather Information Processing System!" << endl;
		cout << endl;
	
		int n = 1;
		
		//put all the options in array
		string options[] = {"Read in and process a configuration file", "Display city map", "Display cloud coverage map (cloudiness index)", "Display cloud coverage map (LMH symbols)", "Display atmospheric pressure map (pressure index)", "Display atmospheric pressure map (LMH symbols)", "Show weather forecast summary report", "Quit", "Display map of city names and surrounding areas"};
	
		for(string option: options){
			cout << n << ")\t" << option << endl;
			n++;
		}
		
		//asking user what option he wants to choose
		cout << "\nPlease enter your choice: ";
		
		//if user typed in the int type as input
		if(cin >> userChoice)switch(userChoice){
		case 1:
			cout << endl;
			cout << "[" << options[0] << "]\n" << endl;
			readConfig();
			if(read_allFiles == true){
			config_read = true;
			}
			cout << endl;
			break;
		case 2:
			cin.ignore();
			cout << endl;
			if(config_read == true){
			city_display = true;
			cout << "[" << options[1] << "]\n" << endl;
			drawGrid(gridX_start, gridX_end, gridY_start, gridY_end, cityLocation_y, cityLocation_x, cityId, dummy_str);
			cout << endl;
			
			cout << "Please <enter> to go back to main menu...";
			getline(cin,goBack);
			
			city_display = false;
			}
			else{
				cout << "Config file hasn't been read.\nRead in config file first." << endl;
			}
			cout << endl;
			break;
		case 3:
			cin.ignore();
			cout << endl;
			if(config_read == true){
			cout << "[" << options[2] << "]\n" << endl;
			drawGrid(gridX_start, gridX_end, gridY_start, gridY_end, cloudCover_y, cloudCover_x, cloudCover_content, dummy_str);
			cout << endl;
			cout << "Please <enter> to go back to main menu...";
			getline(cin,goBack);
			
			}
			else{
				cout << "Config file hasn't been read.\nRead in config file first." << endl;
			}
			cout << endl;
			break;
		case 4:
			cin.ignore();
			cout << endl;
			if(config_read == true){
			cout << "[" << options[3] << "]\n" << endl;
			drawGrid(gridX_start, gridX_end, gridY_start, gridY_end, cloudCover_y, cloudCover_x, dummy_int, cloudCover_LMH);
			cout << endl;
			cout << "Please <enter> to go back to main menu...";
			getline(cin,goBack);
			
			}
			else{
				cout << "Config file hasn't been read.\nRead in config file first." << endl;
			}
			cout << endl;
			break;
		case 5:
			cin.ignore();
			cout << endl;
			if(config_read == true){
			cout << "[" << options[4] << "]\n" << endl;
			drawGrid(gridX_start, gridX_end, gridY_start, gridY_end, pressure_y, pressure_x, pressure_content, dummy_str);
			cout << endl;
			cout << "Please <enter> to go back to main menu...";
			getline(cin,goBack);
			
			}
			else{
				cout << "Config file hasn't been read.\nRead in config file first." << endl;
			}
			cout << endl;
			break;
		case 6:
			cin.ignore();
			cout << endl;
			if(config_read == true){
			cout << "[" << options[5] << "]\n" << endl;
			drawGrid(gridX_start, gridX_end, gridY_start, gridY_end, pressure_y, pressure_x, dummy_int, pressure_LMH);
			cout << endl;
			cout << "Please <enter> to go back to main menu...";
			getline(cin,goBack);
			
			}
			else{
				cout << "Config file hasn't been read.\nRead in config file first." << endl;
			}
			cout << endl;
			break;
		case 7:
			cin.ignore();
			cout << endl;
			if(config_read == true){
			cout << "[" << options[6] << "]\n" << endl;
			summary();
			cout << endl;
			cout << "Please <enter> to go back to main menu...";
			getline(cin,goBack);
			
			}
			else{
				cout << "Config file hasn't been read.\nRead in config file first." << endl;
			}
			cout << endl;
			break;
		case 8:
			cout << endl;
			cout << "[" << options[7] << "]\n" << endl;
			quit = true;
			break;
		case 9:
			cin.ignore();
			cout << endl;
			if(config_read == true){
			cout << "[" << options[8] << "]\n" << endl;
			showReport = false;
			summary();
			showReport = true;
			cout << endl;
			implementation(gridX_start, gridX_end, gridY_start, gridY_end, cityLocation_y, cityLocation_x, surrounding_y, surrounding_x, cityName);
			cout << endl;
			cout << "Please <enter> to go back to main menu...";
			getline(cin,goBack);
			
			}
			else{
				cout << "Config file hasn't been read.\nRead in config file first." << endl;
			}
			cout << endl;
			break;
		default:
			cout << endl;
			cout << "Invalid number. Please try again.\n" << endl;
			break;
		}
		//if user typed in string instead of int type
		else{
			cout << endl;
			cout << "Invalid input. Please try again.\n" << endl;
			cin.clear();
			cin.ignore(1000, '\n');
		}
	}
	
}


//splitting the string
void split(string str, char seperator){
	int startIndex = 0, endIndex = 0;
	//splitting the string to chars
	for(int i = 0; i <= str.size(); i++){
		//if the char is equal to seperator or is the end of the string
		if(str[i] == seperator || i == str.size()){
			//make endIndex the index where we find the seperator
			endIndex = i;
			string temp;
			//adding all the previous chars excluding the seperator into a string
			temp.append(str, startIndex, endIndex - startIndex);
			//adding the string to a vector to use it later
			strings.push_back(temp);
			//change the start for the next string to the index after the seperator or where we ended
			startIndex = endIndex + 1;
		}
	}
}

//read files inside config.txt and save the datas
bool readFile(string filename)
{	
	bool fileFound = false;
	
	strings.clear();
	vector<string> dataItems;
	ifstream file;
	
	//to check the first line of the file to differentiate the file
	bool checkFile = false;
	
	//if the file being read is citylocation this will change to true
	bool cityLocation = false;
	
	file.open(filename);
	
	if(file.good())
	{
		//add the amount of file successfully read from config.txt
		file_found++;
		fileFound = true;
	
		while(! file.eof()){
			
			while(getline(file, data)){
				if(!checkFile){
					//finding the first '-' in file
					size_t finder = data.find("-");
					//finding the second '-' and if it exits its cityLocation file
					if(finder = data.find("-",finder+1) != string::npos){
					cityLocation = true;
					}
					checkFile = true;
					
				}
				
				//splitting the data using the split function
				char separator = '-';
				split(data, separator);
				
			}
			
		}
		for(int i = 0; i < strings.size(); i++){
			//removing the unecessary chars from the data
			if(strings[i].find("[") != string::npos){
			strings[i].erase(remove(strings[i].begin(), strings[i].end(), ' '), strings[i].end());
			strings[i].erase(remove(strings[i].begin(), strings[i].end(), '['), strings[i].end());
			strings[i].erase(remove(strings[i].begin(), strings[i].end(), ']'), strings[i].end());
			
			//splitting the string using ','
			int sign = strings[i].find(",");

			//the front of the ',' is x coordinate
			string x_coordinate_str = strings[i].substr(0, sign);
			int x_coordinate = stoi(x_coordinate_str);
					
			//the back of ',' is y coordinate	
			string y_coordinate_str = strings[i].substr(sign + 1);
			int y_coordinate = stoi(y_coordinate_str);
			
			//saving the coordinates corresponding to the file names
			if(cityLocation == true){
				cityLocation_x.push_back(x_coordinate);
				cityLocation_y.push_back(y_coordinate);
			}
			else if(cloudCover_added == false){
				cloudCover_x.push_back(x_coordinate);
				cloudCover_y.push_back(y_coordinate);
			}
			else{
				pressure_x.push_back(x_coordinate);
				pressure_y.push_back(y_coordinate);
			}

			
			}	
		}
		
		//for citylocation it only saves the cityId and cityName data from the file
		if(cityLocation == true){
			for(int i = 1; i < strings.size(); i+=3){
			
				int cityId_int = stoi(strings[i]);
				cityId.push_back(cityId_int);
				
				stringstream ss;
				string cityName_str;
				ss << strings[i+1];
				ss >> cityName_str;
				
				cityName.push_back(cityName_str);
			}
		}
		//for cloud cover and pressure saves the values and calculated LMH according to those values
		else{
			for(int i = 1; i < strings.size(); i+=2){
				if(cloudCover_added == false){
					string cloudCover_LMH_str = cal_LMH(stoi(strings[i]));
					cloudCover_LMH.push_back(cloudCover_LMH_str);
					
					int cloud_content_int = stoi(strings[i]);
					cloudCover_content.push_back(cloud_content_int);
				}
				else{
					string pressure_LMH_str = cal_LMH(stoi(strings[i]));
					pressure_LMH.push_back(pressure_LMH_str);
					
					int pressure_cotent_int = stoi(strings[i]);
					pressure_content.push_back(pressure_cotent_int);
				}
			}
		}
		
	
	}
	
	
	
	file.close();
	
	//to add pressure to its corresponding file instead of the same file as cloudCover
	if(cityLocation == false){
		cloudCover_added = true;
	}
	return fileFound;
	
}

//drawing the grids based on the arguments
void drawGrid(int gridX_start, int gridX_end, int gridY_start, int gridY_end ,vector<int> vect_a, vector<int> vect_b, vector<int> c, vector<string> d)
{	
	bool vect_int = true;
	
	//differentiaing between string and int vector
	//one of them will be null and we will know which one to use for the grid
	if(c.empty()){
		vect_int = false;
		
	}
	else if(d.empty()){
		vect_int = true;
	}
	
	//gridX and gridY range read from config.txt
	gridX = gridX_end + 1;
	gridY = gridY_end + 1;
	
	//creating the 2d array based on the data read
	string array[gridY][gridX];
	
	//assigning the values in 2d array with the right index
	//for int vector
	if(vect_int == true){
		for (int i = 0; i < vect_a.size(); i++){
			//for city map
			if(city_display == true){
				//changing the int to string to display
				stringstream ss;
				ss << c[i];
				string c_str;
				ss >> c_str;
				array[vect_a[i]][vect_b[i]] = c_str;
			}
			//for cloud and pressure map
			else{
				if(0 <= c[i] and c[i] < 10){
					array[vect_a[i]][vect_b[i]] = "0";
				}
				else if(10 <= c[i] and c[i] < 99){
					stringstream ss;
					ss << c[i];
					string c_str;
					ss >> c_str;
					array[vect_a[i]][vect_b[i]] = c_str[0];
				}
			}
		}
	}
	//for string vector
	else{
		for (int i = 0; i < vect_a.size(); i++){
			array[vect_a[i]][vect_b[i]] = d[i];
		}
	}
	
	
	//drawing the grid in terminal
	cout << setw(6);
	
	for(int i = gridX_start; i < gridX + 2; i++){
		cout << "#" << setw(3);
	}
	cout << endl;
	
	for(int i = gridY - 1; i >= gridY_start; i--){
		cout << setw(3) << i << setw(3) << "#" << setw(3);
		for(int j = gridX_start; j < gridX; j++){
			cout << array[i][j] << setw(3);
		}
		cout << "#";
		cout << endl;
	}
	
	cout << setw(6);
	
	for(int i = gridX_start; i < gridX + 2; i++){
		cout << "#" << setw(3);
	}
	cout << endl;
	cout << setw(9);
	for(int i = gridX_start; i < gridX; i++){
		cout << i << setw(3);
	}
	cout << endl;
}

void implementation(int gridX_start, int gridX_end, int gridY_start, int gridY_end ,vector<int> vect_a, vector<int> vect_b, vector<int> vect_c, vector<int> vect_d, vector<string> e)
{
	
	//gridX and gridY range read from config.txt
	gridX = gridX_end + 1;
	gridY = gridY_end + 1;
	
	//creating the 2d array based on the data read
	string array[gridY][gridX];
	
	for(int i = 0; i < vect_c.size(); i++){
		array[vect_c[i]][vect_d[i]] = "s";
	}
	
	for (int i = 0; i < vect_a.size(); i++){
		array[vect_a[i]][vect_b[i]] = e[i][0];
	}
	
	
	//drawing the grid in terminal
	cout << setw(6);
	
	for(int i = gridX_start; i < gridX + 2; i++){
		cout << "#" << setw(3);
	}
	cout << endl;
	
	for(int i = gridY - 1; i >= gridY_start; i--){
		cout << setw(3) << i << setw(3) << "#" << setw(3);
		for(int j = gridX_start; j < gridX; j++){
			cout << array[i][j] << setw(3);
		}
		cout << "#";
		cout << endl;
	}
	
	cout << setw(6);
	
	for(int i = gridX_start; i < gridX + 2; i++){
		cout << "#" << setw(3);
	}
	cout << endl;
	cout << setw(9);
	for(int i = gridX_start; i < gridX; i++){
		cout << i << setw(3);
	}
	cout << endl;
}

