#ifndef _EXCEPTION_H_
#define	_EXCEPTION_H_ 

#include <exception>

class syntaxError : public std::exception {

};

class quitMiniSQL : public std::exception {

};

class cannotOpenFile : public std::exception {

};

#endif
