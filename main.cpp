#include "seamCarving.h"

using namespace cv;
using namespace std;




int main(int ac, char** av)
{
	// Detection 하기 위해 필요한 데이터
	int corner = 0; // 꼭짓점의 개수.


	// Get Image
	Mat img;
	img = imread("./2.jpg");

	int new_rows, new_cols;
	int width = img.cols;
	int height = img.rows;
	cout << "Original width: " << img.cols << "\n";
	cout << "Original height: " << img.rows << "\n";
	cout << "Enter new width: ";
	cin >> new_cols;
	cout << "Enter new height: ";
	cin >> new_rows;

	for (int i = 0; i < width - new_cols; i++) {
		//seamCarving(img, 0);
		verticalSeamCarving(img);
	}
	for (int i = 0; i < height - new_rows; i++) {
		//seamCarving(img, 1);
		horizontalSeamCarving(img);
	}




	// Print Image
	imwrite("result.jpg", img);
	imshow("img1", img);

	::waitKey(0);
	return 0;
}



void verticalSeamCarving(Mat& img) {

	// Make energy container
	vector<vector<double>> energyArr = calculateEnergyArr(img);

	// minArr is mininum cost of path, equals to sum of the evergy of the path.
	// minArr[y][x] is calculated by
	// energyArr[0][someIndex0] +  energyArr[1][someIndex1] +  energyArr[2[someIndex2] + ...  energyArr[y][x]
	vector<vector<int>> minArr(img.rows, vector<int>(img.cols, 0));
	vector<vector<int>> previousIndexArr(img.rows, vector<int>(img.cols, 0));

	// Seam Identification
	// init minArr[0]
	for (int x = 0; x < img.cols; x++) {
		minArr[0][x] = energyArr[0][x];
	}

	for (int y = 1; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			// choose minimum path cost from 3 minArr element of upper row
			int previousIndex = chooseMinColumn(minArr, y-1 , x - 1, x + 2);
			minArr[y][x] = minArr[y-1][previousIndex]+ energyArr[y][x];
			previousIndexArr[y][x] = previousIndex;

		}
	}

	// get start Index of last row
	int removalIndex = 0;
	for (int i = 1; i < img.cols; i++) {
		if (minArr[img.rows - 1][removalIndex] > minArr[img.rows - 1][i])
			removalIndex = i;
	}

	// Seam removal.
	Mat output(img.rows, img.cols - 1, CV_8UC3);

	// Seam removal starts from last row
	for (int y = img.rows - 1; y >= 0; y--) {
		for (int x = 0; x < img.cols; x++) {

			if (x == removalIndex) {
				img.at<Vec3b>(y, x)[0] = 0;
				img.at<Vec3b>(y, x)[1] = 0;
				img.at<Vec3b>(y, x)[2] = 255;
			}
			if (x >= removalIndex) {
				if (x != img.cols - 1) {
					output.at<Vec3b>(y, x) = img.at<Vec3b>(y, x + 1);
				}
			}
			else {
				output.at<Vec3b>(y, x) = img.at<Vec3b>(y, x);
			}
		}

		if (y > 1) {
			removalIndex = previousIndexArr[y][removalIndex];
		}
	}
	cv::imshow("seam image", img);
	cv::waitKey(1);
	img = output;
}



void horizontalSeamCarving(Mat& img) {

	// Make energy container
	vector<vector<double>> energyArr = calculateEnergyArr(img);

	// minArr is mininum cost of path, equals to sum of the evergy of the path.
	// minArr[y][x] is calculated by
	// energyArr[someIndex0][0] +  energyArr[someIndex1][1] +  energyArr[someIndex2][2] + ...  energyArr[y][x]
	vector<vector<int>> minArr(img.rows, vector<int>(img.cols, 0));
	vector<vector<int>> previousIndexArr(img.rows, vector<int>(img.cols, 0));

	// Seam Identification
	// init minArr[0]
	for (int y = 0; y < img.rows; y++) {
		minArr[y][0] = energyArr[y][0];
	}

	for (int x = 1; x < img.cols; x++) {
		for (int y = 0; y < img.rows; y++) {
			// choose minimum path cost from 3 minArr element of left(previous) column
			int previousIndex = chooseMinRow(minArr, x-1, y - 1, y + 2);
			minArr[y][x] = minArr[previousIndex][x-1] + energyArr[y][x];
			previousIndexArr[y][x] = previousIndex;
		}
	}

	// get start Index of last column
	int removalIndex = 0;
	for (int i = 1; i < img.rows; i++) {
		if (minArr[removalIndex][img.cols-1] > minArr[i][img.cols-1])
			removalIndex = i;
	}

	// Seam removal.
	Mat output(img.rows - 1, img.cols, CV_8UC3);

	// Seam removal starts from last column
	for (int x = img.cols - 1; x >= 0; x--) {
		for (int y = 0; y < img.rows; y++) {

			if (y == removalIndex) {
				img.at<Vec3b>(y, x)[0] = 0;
				img.at<Vec3b>(y, x)[1] = 0;
				img.at<Vec3b>(y, x)[2] = 255;
			}
			if (y >= removalIndex) {
				if (y != img.rows - 1) {
					output.at<Vec3b>(y, x) = img.at<Vec3b>(y + 1, x);
				}
			}
			else {
				output.at<Vec3b>(y, x) = img.at<Vec3b>(y, x);
			}
		}

		if (x > 1) {
			removalIndex = previousIndexArr[removalIndex][x];
		}
	}
	cv::imshow("seam image", img);
	cv::waitKey(1);
	img = output;
}



int chooseMinColumn(vector<vector<int>> &arr, int row, int start, int end) {
	if (start > end) return 0;

	start = max(start, 0);
	end = min(end, (int)arr[0].size());

	int ret = start;

	for (int i = start; i < end; i++) {
		if (arr[row][i] < arr[row][ret]) {
			ret = i;
		}
	}
	return ret;
}


int chooseMinRow(vector<vector<int>> &arr, int column, int start, int end) {
	if (start > end) return 0;

	start = max(start, 0);
	end = min(end, (int)arr.size());

	int ret = start;

	for (int i = start; i < end; i++) {
		if (arr[i][column] < arr[ret][column]) {
			ret = i;
		}
	}
	return ret;
}


// Energy Calculation
vector<vector<double>> calculateEnergyArr(Mat& img) {

	vector<vector<double>> energyArr(img.rows, vector<double>(img.cols, 0));

	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			double energy = 0;
			if (x == 0) {
				Vec3b& colorX1 = img.at<Vec3b>(y, x + 1);
				Vec3b& colorX2 = img.at<Vec3b>(y, img.cols - 1);
				energy += pow(colorX1[0] - colorX2[0], 2) + pow(colorX1[1] - colorX2[1], 2) + pow(colorX1[2] - colorX2[2], 2);
			}
			else if (x + 1 == img.cols) {
				Vec3b& colorX1 = img.at<Vec3b>(y, x - 1);
				Vec3b& colorX2 = img.at<Vec3b>(y, 0);
				energy += pow(colorX1[0] - colorX2[0], 2) + pow(colorX1[1] - colorX2[1], 2) + pow(colorX1[2] - colorX2[2], 2);
			}
			else {
				Vec3b colorX1 = img.at<Vec3b>(y, x - 1);
				Vec3b colorX2 = img.at<Vec3b>(y, x + 1);
				energy += pow(colorX1[0] - colorX2[0], 2) + pow(colorX1[1] - colorX2[1], 2) + pow(colorX1[2] - colorX2[2], 2);
			}
			if (y == 0) {
				Vec3b colorY1 = img.at<Vec3b>(y + 1, x);
				Vec3b colorY2 = img.at<Vec3b>(img.rows - 1, x);
				energy += pow(colorY1[0] - colorY2[0], 2) + pow(colorY1[1] - colorY2[1], 2) + pow(colorY1[2] - colorY2[2], 2);
			}
			else if (y + 1 == img.rows) {
				Vec3b colorY1 = img.at<Vec3b>(y - 1, x);
				Vec3b colorY2 = img.at<Vec3b>(0, x);
				energy += pow(colorY1[0] - colorY2[0], 2) + pow(colorY1[1] - colorY2[1], 2) + pow(colorY1[2] - colorY2[2], 2);
			}
			else {
				Vec3b colorY1 = img.at<Vec3b>(y - 1, x);
				Vec3b colorY2 = img.at<Vec3b>(y + 1, x);
				energy += pow(colorY1[0] - colorY2[0], 2) + pow(colorY1[1] - colorY2[1], 2) + pow(colorY1[2] - colorY2[2], 2);
			}
			energy = sqrt(energy);
			energyArr[y][x] = energy;
		}
	}
	return energyArr;
}


void seamCarving(Mat& img, int n) {

	// Make energy container
	vector<vector<double>> energyArr = calculateEnergyArr(img);


	//vertical seam carving
	if (n == 0) {
		// Seam Identification
		vector<vector<vector<int>>> minArr(img.rows, vector<vector<int>>(img.cols, vector<int>(3, 0)));
		for (int x = 0; x < img.cols; x++) {
			minArr[0][x][0] = energyArr[0][x];
			minArr[0][x][1] = x;
			minArr[0][x][2] = 0;
		}
		for (int y = 1; y < img.rows; y++) {
			for (int x = 0; x < img.cols; x++) {
				if (minArr[y - 1][x][1] == 0) {
					if (energyArr[y][minArr[y - 1][x][1]] > energyArr[y][minArr[y - 1][minArr[y - 1][x][1]][1] + 1]) {
						minArr[y][x][0] = energyArr[y][minArr[y - 1][x][1] + 1] + minArr[y - 1][x][0];
						minArr[y][x][1] = minArr[y - 1][x][1] + 1;
						minArr[y][x][2] = y;
					}
					else {
						minArr[y][x][0] = energyArr[y][minArr[y - 1][x][1]] + minArr[y - 1][x][0];
						minArr[y][x][1] = minArr[y - 1][x][1];
						minArr[y][x][2] = y;
					}
				}
				else if (minArr[y - 1][x][1] == img.cols - 1) {
					if (energyArr[y][minArr[y - 1][x][1] - 1] < energyArr[y][minArr[y - 1][x][1]]) {
						minArr[y][x][0] = energyArr[y][minArr[y - 1][x][1] - 1] + minArr[y - 1][x][0];
						minArr[y][x][1] = minArr[y - 1][x][1] - 1;
						minArr[y][x][2] = y;
					}
					else {
						minArr[y][x][0] = energyArr[y][minArr[y - 1][x][1]] + minArr[y - 1][x][0];
						minArr[y][x][1] = minArr[y - 1][x][1];
						minArr[y][x][2] = y;
					}
				}
				else {
					if (energyArr[y][minArr[y - 1][x][1] - 1] < energyArr[y][minArr[y - 1][x][1]]) {
						if (energyArr[y][minArr[y - 1][x][1] - 1] < energyArr[y][minArr[y - 1][x][1] + 1]) {
							minArr[y][x][0] = energyArr[y][minArr[y - 1][x][1] - 1] + minArr[y - 1][x][0];
							minArr[y][x][1] = minArr[y - 1][x][1] - 1;
							minArr[y][x][2] = y;
						}
						else {
							minArr[y][x][0] = energyArr[y][minArr[y - 1][x][1] + 1] + minArr[y - 1][x][0];
							minArr[y][x][1] = minArr[y - 1][x][1] + 1;
							minArr[y][x][2] = y;
						}
					}
					else {
						if (energyArr[y][minArr[y - 1][x][1]] < energyArr[y][minArr[y - 1][x][1] + 1]) {
							minArr[y][x][0] = energyArr[y][minArr[y - 1][x][1]] + minArr[y - 1][x][0];
							minArr[y][x][1] = minArr[y - 1][x][1];
							minArr[y][x][2] = y;
						}
						else {
							minArr[y][x][0] = energyArr[y][minArr[y - 1][x][1] + 1] + minArr[y - 1][x][0];
							minArr[y][x][1] = minArr[y - 1][x][1] + 1;
							minArr[y][x][2] = y;
						}
					}
				}
			}
		}
		// Seam removal
		int min_index = 0;
		for (int x = 0; x < img.cols; x++) {
			if (minArr[img.rows - 1][min_index][0] > minArr[img.rows - 1][x][0])
				min_index = x;
		}
		Mat output(img.rows, img.cols - 1, CV_8UC3);
		for (int y = 0; y < img.rows; y++) {
			for (int x = 0; x < img.cols; x++) {
				if (x == minArr[y][min_index][1]) {
					img.at<Vec3b>(y, x)[0] = 0;
					img.at<Vec3b>(y, x)[1] = 0;
					img.at<Vec3b>(y, x)[2] = 255;
				}
				if (x >= minArr[y][min_index][1]) {
					if (x != img.cols - 1) {
						output.at<Vec3b>(y, x) = img.at<Vec3b>(y, x + 1);
					}
				}
				else {
					output.at<Vec3b>(y, x) = img.at<Vec3b>(y, x);
				}
			}
		}
		cv::imshow("seam image", img);
		cv::waitKey(1);
		img = output;
	}

	//horizontal seam carving
	else if (n == 1) {
		// Seam Identification
		vector<vector<vector<int>>> minArr(img.rows, vector<vector<int>>(img.cols, vector<int>(3, 0)));
		for (int x = 0; x < img.rows; x++) {
			minArr[x][0][0] = energyArr[x][0];
			minArr[x][0][1] = 0;
			minArr[x][0][2] = x;
		}
		for (int x = 1; x < img.cols; x++) {
			for (int y = 0; y < img.rows; y++) {
				if (minArr[y][x - 1][2] == 0) {
					if (energyArr[minArr[y][x - 1][2]][x] > energyArr[minArr[y][x - 1][2] + 1][x]) {
						minArr[y][x][0] = energyArr[minArr[y][x - 1][2] + 1][x] + minArr[y][x - 1][0];
						minArr[y][x][1] = x;
						minArr[y][x][2] = minArr[y][x - 1][2] + 1;
					}
					else {
						minArr[y][x][0] = energyArr[minArr[y][x - 1][2]][x] + minArr[y][x - 1][0];
						minArr[y][x][1] = x;
						minArr[y][x][2] = minArr[y][x - 1][2];
					}
				}
				else if (minArr[y][x - 1][2] == img.rows - 1) {
					if (energyArr[minArr[y][x - 1][2] - 1][x] < energyArr[minArr[y][x - 1][2]][x]) {
						minArr[y][x][0] = energyArr[minArr[y][x - 1][2] - 1][x] + minArr[y][x - 1][0];
						minArr[y][x][1] = x;
						minArr[y][x][2] = minArr[y][x - 1][2] - 1;
					}
					else {
						minArr[y][x][0] = energyArr[minArr[y][x - 1][2]][x] + minArr[y][x - 1][0];
						minArr[y][x][1] = x;
						minArr[y][x][2] = minArr[y][x - 1][2];
					}
				}
				else {
					if (energyArr[minArr[y][x - 1][2] - 1][x] < energyArr[minArr[y][x - 1][2]][x]) {
						if (energyArr[minArr[y][x - 1][2] - 1][x] < energyArr[minArr[y][x - 1][2] + 1][x]) {
							minArr[y][x][0] = energyArr[minArr[y][x - 1][2] - 1][x] + minArr[y][x - 1][0];
							minArr[y][x][1] = x;
							minArr[y][x][2] = minArr[y][x - 1][2] - 1;
						}
						else {
							minArr[y][x][0] = energyArr[minArr[y][x - 1][2] + 1][x] + minArr[y][x - 1][0];
							minArr[y][x][1] = x;
							minArr[y][x][2] = minArr[y][x - 1][2] + 1;
						}
					}
					else {
						if (energyArr[minArr[y][x - 1][2]][x] < energyArr[minArr[y][x - 1][2] + 1][x]) {
							minArr[y][x][0] = energyArr[minArr[y][x - 1][2]][x] + minArr[y][x - 1][0];
							minArr[y][x][1] = x;
							minArr[y][x][2] = minArr[y][x - 1][2];
						}
						else {
							minArr[y][x][0] = energyArr[minArr[y][x - 1][2] + 1][x] + minArr[y][x - 1][0];
							minArr[y][x][1] = x;
							minArr[y][x][2] = minArr[y][x - 1][2] + 1;
						}
					}
				}
			}
		}
		// Seam removal
		int min_index = 0;
		for (int x = 0; x < img.rows; x++) {
			if (minArr[min_index][img.cols - 1][0] > minArr[x][img.cols - 1][0])
				min_index = x;
		}
		Mat output(img.rows - 1, img.cols, CV_8UC3);
		for (int x = 0; x < img.cols; x++) {
			for (int y = 0; y < img.rows; y++) {
				if (y == minArr[min_index][x][2]) {
					img.at<Vec3b>(y, x)[0] = 0;
					img.at<Vec3b>(y, x)[1] = 0;
					img.at<Vec3b>(y, x)[2] = 255;
				}
				if (y >= minArr[min_index][x][2]) {
					if (y != img.rows - 1) {
						output.at<Vec3b>(y, x) = img.at<Vec3b>(y + 1, x);
					}
				}
				else {
					output.at<Vec3b>(y, x) = img.at<Vec3b>(y, x);
				}
			}
		}
		cv::imshow("seam image", img);
		cv::waitKey(1);
		img = output;
	}
}