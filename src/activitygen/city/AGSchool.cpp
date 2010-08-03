#include <iostream>
#include <string>
#include "AGSchool.h"
#include "AGPosition.h"

using namespace std;

void
AGSchool::print()
{
	cout << "- school: " << " placeNbr=" << capacity << " hours=[" << opening << ";" << closing << "] ages=[" << beginAge << ";" << endAge << "]" << endl;
}

int
AGSchool::getPlaces()
{
	return capacity;
}

bool
AGSchool::addNewChild()
{
	if(capacity > 0)
	{
		--capacity;
		return true;
	}
	return false;
}

bool
AGSchool::removeChild()
{
	if(capacity < initCapacity)
	{
		++capacity;
		return true;
	}
	return false;
}

bool
AGSchool::acceptThisAge(int age)
{
	if(age <= endAge && age >= beginAge)
		return true;
	return false;
}

int
AGSchool::getBeginAge()
{
	return beginAge;
}

int
AGSchool::getEndAge()
{
	return endAge;
}

AGPosition
AGSchool::getPosition()
{
	return location;
}

int
AGSchool::getClosingHour()
{
	return closing;
}

int
AGSchool::getOpeningHour()
{
	return opening;
}
