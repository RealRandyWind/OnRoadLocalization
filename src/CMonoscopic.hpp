#ifndef CMONOSCOPIC_HPP
#define CMONOSCOPIC_HPP
#include "IComputation.hpp"


class CMonoscopic;
class CMonoscopic : public IComputation
{
public:
	CMonoscopic();
	~CMonoscopic();

	virtual void Run();
};
#endif