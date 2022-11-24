#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

void seamCarving(Mat& img, int n);

vector<vector<double>> calculateEnergyArr(Mat& img);
void verticalSeamCarving(Mat& img);
void horizontalSeamCarving(Mat& img);

int chooseMinRow(vector<vector<int>> &arr, int column, int start, int end);
int chooseMinColumn(vector<vector<int>> &arr, int row, int start, int end);