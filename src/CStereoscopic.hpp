#ifndef CSTEREOSCOPIC_HPP
#define CSTEREOSCOPIC_HPP
#include "IComputation.hpp"


class CStereoscopic;
class CStereoscopic : public IComputation
{
public:
	CStereoscopic();
	~CStereoscopic();

	virtual void Run();
};
#endif