#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <math.h>
using namespace std;

/*
  
  Things that we can do in the future:
   - Remove or mark stability data with error/derivation bigger than a Threshold  
   - Add real timestamps
*/


const string COLOR_MATCH_FILE_NAME = "colourmatch.csv";
const double FIRST_WAVELENGTH = 390.0;
const double LAST_WAVELENGTH = 830.0;
const int COLOR_MATCH_LENGHT = 4401;

double Ax[COLOR_MATCH_LENGHT];
double Ay[COLOR_MATCH_LENGHT];
double Az[COLOR_MATCH_LENGHT];

vector<double> wavelenghts;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						Utilities
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void printUsage() {
	cout << endl;
	cout << "Usage: " << endl;
	cout << endl;
	cout << "  colouranalyzer <data_directory>" << endl;
	cout << endl;
	cout << "  Input:" << endl;
	cout << "      Wavelength Values: <data_directory>\\wavelengths.csv" << endl;
	cout << "      Intensity Values: <data_directory>\\intensities.csv" << endl;
	cout << endl;
	cout << "  Output:" << endl;
	cout << "      XY values: <data_directory>\\xy.csv" << endl;
	cout << "      Stability on time values: <data_directory>\\stability.csv" << endl;
	cout << endl << endl;
	exit(-1);
}

int wavelength2index(double wavelength) {
	return (int)round(wavelength * 10) - (int)(FIRST_WAVELENGTH * 10);
}

/* Split string */
vector<double> split(const string& line) {
	vector<double> data;
	int pos = 0, currPos = 0, prePos = 0;
	do {
		pos = line.find(',', currPos);
		if (pos >= 0) {
			data.push_back(stod(line.substr(currPos, pos - currPos)));
			currPos = pos + 1;
		}
	} while (pos >= 0);
	data.push_back(stod(line.substr(currPos, pos - currPos)));
	return data;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						Read Data from Files
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Read the Color Match table*/
void readColorMatch() {
	ifstream colorMatchFile(COLOR_MATCH_FILE_NAME);
	if (colorMatchFile.is_open()) {
		string line;
		cout << "Reading color match data..." << endl;
		while (!colorMatchFile.eof()) {
			getline(colorMatchFile, line, '\n');
			if (line != "") {
				vector<double> data = split(line);
				double wavelength = data[0];
				int wavelengthIdx = wavelength2index(wavelength);
				Ax[wavelengthIdx] = data[1];
				Ay[wavelengthIdx] = data[2];
				Az[wavelengthIdx] = data[3];
				//cout << wavelength << " " << Ax[wavelengthIdx] << " ";
				//cout << Ay[wavelengthIdx] << " " << Az[wavelengthIdx] << " " << endl;
			}
		}
		colorMatchFile.close();
	}
	else {
		cout << "Cannot read the file " << COLOR_MATCH_FILE_NAME << endl;
		printUsage();
	}
}

/* Read the Wavelength index file */
void readWavelengthsFile(const string& wavelengthFileName) {
	ifstream wavelengthFile(wavelengthFileName);

	if (wavelengthFile.is_open()) {
		string line;
		cout << "Reading wavelengths data..." << endl;
		getline(wavelengthFile, line, '\n'); //read Head
		while (!wavelengthFile.eof()) {
			getline(wavelengthFile, line, '\n');
			if (line != "") {
				vector<double> data = split(line);
				wavelenghts.push_back(data[1]);
			}
		}
		wavelengthFile.close();
	}
	else {
		cout << "Cannot read the file " << wavelengthFileName << endl;
		printUsage();
	}
}


/* Read the Wavelength index file */
vector<vector<double>> readIntensitiesFile(const string& intensitiesFileName) {
	ifstream intensitiesFile(intensitiesFileName);
	pair<vector<double>, vector<double>> xyVectors;
	vector<vector<double>> data;

	if (intensitiesFile.is_open()) {
		string line;
		cout << "Reading intensities data";
		while (!intensitiesFile.eof()) {
			getline(intensitiesFile, line, '\n');
			if (line != "") {
				cout << ".";
				vector<double> values = split(line);
				data.push_back(values);
			}
		}
		intensitiesFile.close();
		cout << endl;
	}
	else {
		cout << "Cannot read the file " << intensitiesFileName << endl;
		printUsage();
	}

	return data;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						Write Data into Files
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*write xy file*/
void writeXYFile(string filename, vector<double> xv, vector<double> yv) {
	ofstream outfile;
	outfile.open(filename);
	outfile << fixed;
	outfile << setprecision(14);
	for (size_t i = 0; i < xv.size(); i++) {
		outfile << xv[i] << "," << yv[i] << endl;
		//cout << " x: " << xv[i] << "  y: " << yv[i] << endl;
	}
	outfile.close();
}

/*write xy file*/
void writeStabilityFile(string filename, vector<double> stability) {
	ofstream outfile;
	outfile.open(filename);
	outfile << fixed;
	outfile << setprecision(14);
	for (size_t i = 0; i < stability.size(); i++) {
		outfile << stability[i] << endl;
		//cout << "Stability: " << stability[i] << endl;
	}
	outfile.close();
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						Calc Routines
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Used to remove noise from data
vector<vector<double>> averangeFilter(const vector<vector<double>>& data) {
	//TODO: Implement it - I had not enough time to do it - Sorry.

	return data;
}

/*
Find the value of x and y
     Return the x, y pair
*/
pair<double, double> findXY(const vector<double>& intensities) {
	//find X, Y and Z
	double X = 0;
	double Y = 0;
	double Z = 0;
	int wavelengthIdx;

	//We are interested in the relation between X, Y and Z, so we can use a simple Sum to solve the Integral instead to solve it using a more complex method like the Trapezoidal rule
	for (size_t i = 0; i < intensities.size(); i++) {
		wavelengthIdx = wavelength2index(wavelenghts[i]);
		if (wavelenghts[i] >= FIRST_WAVELENGTH && wavelenghts[i] <= LAST_WAVELENGTH) {
			//cout << wavelenghts[i] << " - " << wavelengthIdx << " - " << intensities[i] << " - A: " << Ax[wavelengthIdx] << endl;
			X = X + (intensities[i] * Ax[wavelengthIdx]);
			Y = Y + (intensities[i] * Ay[wavelengthIdx]);
			Z = Z + (intensities[i] * Az[wavelengthIdx]);
		}
	}
	//cout << " X: " << X << "  Y: " << Y << "  Z: " << Z << endl;

	//Normalize to find x, y
	double norm = X + Y + Z;
	double x = X / norm;
	double y = Y / norm;
	pair<double, double> xy;
	xy.first = x;
	xy.second = y;
	return xy;
}

/*
	Return x_vector, y_vector
*/
pair<vector<double>, vector<double>> findPerceivedColour(const vector<vector<double>>& data) {
	pair<vector<double>, vector<double>> xyVectors;
	vector<double> xv;
	vector<double> yv;

	for (size_t i = 0; i < data.size(); i++) {
		pair<double, double> xy = findXY(data[i]);
		//cout << " x1: " << xy.first << "  y1: " << xy.second << endl;
		xv.push_back(xy.first);
		yv.push_back(xy.second);
	}
	xyVectors.first = xv;
	xyVectors.second = yv;
	return xyVectors;
}

/*
	Return stability_vector
*/
vector<double> findColourStability(const vector<double>& xv, const vector<double>& yv) {
	cout << "Processing Perceived Colour stability..." << endl;
	if (xv.empty() || yv.empty() || xv.size() != yv.size()) {
		cout << endl << "Find Color Stability Error - invalid entries" << endl;
		cout << endl;
		exit(-1);
	}
	//using the first value as reference
	double xyRef = xv[0] * yv[0];
	vector<double> deltas;
	double deltaXY;
	for (size_t i = 0; i < xv.size(); i++) {
		deltaXY = abs(xyRef - (xv[i] * yv[i]));
		deltas.push_back(deltaXY);
	}
	return deltas;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						Main 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* args[]) {
	if (argc < 2) {
		printUsage();
	}
	string dir = args[1];
	cout << fixed;
	cout << setprecision(14);
	readColorMatch();
	readWavelengthsFile(dir + "\\wavelengths.csv");
	vector<vector<double>> intensitiesData = readIntensitiesFile(dir + "\\intensities.csv");
	vector<vector<double>> intensitiesDataFiltered = averangeFilter(intensitiesData); //Remove noise
	pair<vector<double>, vector<double>> perceivedColour = findPerceivedColour(intensitiesDataFiltered);
	vector<double> xv = perceivedColour.first;
	vector<double> yv = perceivedColour.second;
	vector<double> stability = findColourStability(xv, yv);
	writeXYFile(dir + "\\xy.csv", xv, yv);
	writeStabilityFile(dir + "\\stability.csv", stability);
	cout << endl << endl;
	//system("pause");
	return 0;
}

