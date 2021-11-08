//
// Created by dt on 2021/11/8.
//

#ifndef ARKTOOLS_PERSON_H
#define ARKTOOLS_PERSON_H

#include <iostream>
#include <string>

using namespace std;


class Person {

public:
    Person() {
        age = 31;
        printf("Person:Person() called\n");
    };

    virtual ~Person() {
        printf("Person:~Person() called\n");
    }

    virtual int getAge() {
        printf("Person:~getAge() called\n");
        return age;
    }

    virtual void setAge(int age_) {
        age = age_;
    }

private:
    int age;
};


#endif //ARKTOOLS_PERSON_H
