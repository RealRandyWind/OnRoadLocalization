#ifndef ICOMPUTATION_HPP
#define ICOMPUTATION_HPP

class IComputation;
class IComputation
{
public:
	virtual ~IComputation() {};
	virtual void Run() = 0;
};
#endif