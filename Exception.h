#ifndef _EXCEPTION_H_
#define	_EXCEPTION_H_ 

#include <exception>

class syntaxError : public std::exception {

};

class quitMiniSQL : public std::exception {

};

class cannotOpenFile : public std::exception {

};

class tableExists : public std::exception {

};

class tableNotExists : public std::exception {

};

class attributeNotExists : public std::exception {

};

class duplicateIndexOnAttribute : public std::exception {

};

class duplicateIndexName : public std::exception {

};

class nameTooLong : public std::exception {

};

class negativeNum : public std::exception {

};

class tooManyIndex : public std::exception {

};

class indexNotExist : public std::exception {

};

class illegalIdentifier : public std::exception {

};

class targetNotFound : public std::exception {

};

class attributeNotUnique : public std::exception {

};

class tupleTypeConflict : public std::exception {

};

class uniqueConflict : public std::exception {

};

class dataTypeConflict : public std::exception {

};
#endif
