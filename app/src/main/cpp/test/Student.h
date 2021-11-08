//
// Created by dt on 2021/11/8.
//

#ifndef ARKTOOLS_STUDENT_H
#define ARKTOOLS_STUDENT_H

#include <iostream>
#include <string>

using namespace std;

#include "Person.h"


class Student : public Person {
public:
    Student() {
        printf("Student:Student() called\n");
    }

    virtual ~Student() {
        printf("Student:~Student() called\n");
    }

    virtual int getAge() {
        printf("Student:~getAge() called\n");
        return Person::getAge();
    }
};


#endif //ARKTOOLS_STUDENT_H
