//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Kyle Smith
// Version     : 1.0
// Description : 7-1 Project Two
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
using namespace std;

// Convert a string to lower case for case-insensitive comparisions
string toLowerCase(const string& str) {
	string lowerStr = str;
	transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), [](unsigned char c) {
		return tolower(c);
	});
	return lowerStr;
}

// Structure to represent a course
struct Course {
	string courseNumber;
	string courseName;
	vector<string> prereqs;
};

// BST node structure, holding course and pointers
struct treeNode {
	Course course;
	treeNode* left;
	treeNode* right;

	treeNode(Course newCourse) : course(newCourse), left(nullptr), right(nullptr) {}
};

//Define a BST class
class BinarySearchTree {
private:
	treeNode* root;

	// insert a course into BST based on course number
	void insertRecursive(treeNode* node, Course course) {
		if (course.courseNumber < node->course.courseNumber) {
			if (node->left == nullptr) {
				node->left = new treeNode(course);
			}
			else {
				insertRecursive(node->left, course);
			}
		}
		else {
			if (node->right == nullptr) {
				node->right = new treeNode(course);
			}
			else {
				insertRecursive(node->right, course);
			}
		}
	}

	// Search for a course by its course number
	Course* searchRecursive(treeNode* node, const string& courseNumber) {
		if (node == nullptr) return nullptr;

		string nodeCourseLower = toLowerCase(node->course.courseNumber);
		string searchCourseLower = toLowerCase(courseNumber);

		if (nodeCourseLower == searchCourseLower) return &node->course;

		if (searchCourseLower < nodeCourseLower) {
			return searchRecursive(node->left, courseNumber);
		}
		else {
			return searchRecursive(node->right, courseNumber);
		}
	}

	// In-order traversal to print courses in alphanumeric order
	void printInOrder(treeNode* node) {
		if (node != nullptr) {
			printInOrder(node->left);  // Visit left subtree
			cout << node->course.courseNumber << " : " << node->course.courseName << endl;
			printInOrder(node->right);  // Visit right subtree
		}
	}

public:
	BinarySearchTree() : root(nullptr) {}

	void insertCourse(Course course) {
		if (root == nullptr) {
			root = new treeNode(course);
		}
		else {
			insertRecursive(root, course);
		}
	}

	Course* searchCourse(const string& courseNumber) {
		string searchCourseLower = toLowerCase(courseNumber);
		return searchRecursive(root, searchCourseLower);
	}

	void printCourses() {
		if (root == nullptr) {
			cout << endl;
			cout << "================================================" << endl;
			cout << endl;
			cout << "No courses available." << endl;
			cout << endl;
			cout << "================================================" << endl;
			cout << endl;
			return;
		}
		cout << endl;
		cout << "================================================" << endl;
		cout << endl;
		printInOrder(root);
		cout << endl;
		cout << "================================================" << endl;
		cout << endl;
	}
};

// Load courses from a csv file into the BST
void loadCourses(const string& fileName, BinarySearchTree& bst) {
	ifstream file(fileName);
	if (!file.is_open()) {
		cout << endl;
		cout << "================================================" << endl;
		cout << endl;
		cout << "Error: File could not be opened." << endl;
		cout << endl;
		cout << "================================================" << endl;
		cout << endl;
		return;
	}

	vector<Course> allCourses;
	string line;

	//First pass: Parse each line and store course data in a vector
	while (getline(file, line)) {
		stringstream ss(line);
		string token;
		vector<string> tokens;

		while (getline(ss, token, ',')) {
			tokens.push_back(token);
		}

		if (tokens.size() < 2) {
			cout << "Error: Invalid format in file." << endl;
			continue;
		}

		// Create Course object
		Course course;
		course.courseNumber = tokens[0];
		course.courseName = tokens[1];
		for (size_t i = 2; i < tokens.size(); ++i) {
			course.prereqs.push_back(tokens[i]);  // Add prerequisites
		}
		allCourses.push_back(course);
	}
	file.close();

	// Set of valid course numbers for quick lookup
	vector<string> validCourses;
	for (const auto& course : allCourses) {
		validCourses.push_back(course.courseNumber);
	}

	//Second pass: Validate prerequisites before inserting valid courses into BST
	for (const auto& course : allCourses) {
		if (course.prereqs.empty() || (course.prereqs.size() == 1 && course.prereqs[0].find_first_not_of(" \t") == string::npos)) {
			bst.insertCourse(course);  // Insert if course has no prerequisites
		}
	}

	for (const auto& course : allCourses) {
		if (!course.prereqs.empty()) {
			bool invalid = false;
			for (const auto& prereq : course.prereqs) {
				if (find(validCourses.begin(), validCourses.end(), prereq) == validCourses.end()) {
					invalid = true;
					break;
				}
			}

			if (!invalid) {
				bst.insertCourse(course);
			}
		}
	}

}


// Function to print course and prerequisites
void printCourse(BinarySearchTree& bst, const string& courseNumber) {
	string searchCourse = toLowerCase(courseNumber);
	Course* course = bst.searchCourse(courseNumber);

	if (course == nullptr) {
		cout << endl;
		cout << "Course not found in available courses." << endl;
		cout << endl;
	}
	else {
		cout << endl;
		cout << "================================================" << endl;
		cout << endl;
		cout << "Course Number: " << course->courseNumber << endl;
		cout << "Course Name: " << course->courseName << endl;

		if (course->prereqs.empty()) {
			cout << "No prerequisites." << endl;
		}
		else {
			cout << "Prerequisites: ";
			for (const auto& prereq : course->prereqs) {
				cout << prereq << " ";
			}
			cout << endl;
			cout << endl;
			cout << "================================================" << endl;
			cout << endl;
		}
	}
}

// Main function: Display menu and handle user interaction
int main() {
	BinarySearchTree bst; // BST to store courses
	string fileName;
	string courseNumber;
	int userChoice;

	while (true) {
		cout << "------------------------------" << endl;
		cout << "Welcome to the Course Planner." << endl;
		cout << "------------------------------" << endl;
		cout << endl;
		cout << "      " << "1. Load Courses" << endl;
		cout << "      " << "2. Display Courses" << endl;
		cout << "      " << "3. Find Course" << endl;
		cout << "      " << "9. Exit" << endl;
		cout << endl;
		cout << "------------------------------" << endl;
		cout << "What would you like to do? ";
		cin >> userChoice;

		switch (userChoice) {

		case 1:
			cout << endl;
			cout << "Enter file name: ";
			cin.ignore();
			getline(cin, fileName);
			loadCourses(fileName, bst);  // Load course data from file
			break;

		case 2:
			cout << endl;
			cout << "Here are the available courses:" << endl;
			bst.printCourses();  // Print all courses
			break;

		case 3:
			cout << endl;
			cout << "What course would you like to see? ";
			cin.ignore();
			getline(cin, courseNumber);
			printCourse(bst, courseNumber);  // Find and display specific course
			break;

		case 9:
			cout << endl;
			cout << "Thank you for using the Course Planner!" << endl;
			return 0;  // Exit program

		default:
			cout << endl;
			cout << userChoice << " is not a valid option." << endl;
			cout << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}

	return 0;
}