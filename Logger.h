#ifndef LOGGER_H
#define LOGGER_H

#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>

//undefine the costum windows error
#undef ERROR

// three types of errors
enum Type 
{
    WARNING = 0,
    ERROR = 1,
    SUCCESS = 2
};

// stringify the type of reports
const std::string TypeString[3] = {"WARNING", "ERROR", "SUCCESS"};

// main class that accepts the error type
// reports any success and error messages, as well as certain warnings
class Report : public std::ostringstream {
    public:
        Report(Type type);
        ~Report();

        //print to the screen any reports
        void output();

    private:
        // accessor for getting type
        Type _type;
};

// incase of error, output to screen
class ReportError : public Report {
    public:
        ReportError();
        ~ReportError();
};

// class used for debugging, and reports successfull reports
// extends from Report function
class ReportSuccess : public Report {
    public:
        ReportSuccess();
        ~ReportSuccess();

};

// whenever an error encountered by one of the stages throw an error
// accepts the phase, message to display, the file name, and the line number,
// this allows the user to easily fix the mistakes
#define LOG_ERROR(phase, message, filename, linenumber) \
    ReportError().flush() << "in '" << phase <<"' stage at file: '" << filename << "' - " << message << " at line: " << linenumber

//does not terminate the program
//used for debugging purposes
#define LOG_SUCCESS(phase, message, filename, linenumber) \
    ReportSuccess().flush() << "in '" << phase <<"' stage at file: '" << filename << "' - " << message << " at line: " << linenumber

#endif

