#include "Logger.h"

// accepts the type of report
Report::Report(Type type) : _type(type) {}

// destructor
// automatically calls output function
// to display to user
Report::~Report() {
    output();
}

// display the report type and the message from the define function
void Report::output(){
    fprintf(stderr, "%s %s\n", TypeString[_type].c_str(), str().c_str()); 
}

// incase of error
ReportError::ReportError() : Report(ERROR) {}

// incase of error automatically exit the program
ReportError::~ReportError() {
    output();
    exit(1);
}

// with success, do not terminate the program
// input the report type into report class
ReportSuccess::ReportSuccess() : Report(SUCCESS) {}

// automatically call report class
ReportSuccess::~ReportSuccess() {}