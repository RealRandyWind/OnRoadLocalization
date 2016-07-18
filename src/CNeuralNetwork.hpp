#ifndef CNEURALNETWORK_HPP
#define CNEURALNETWORK_HPP
#include "IComputation.hpp"

class CNeuralNetwork;
class CNeuralNetwork : public IComputation
{
public:
	CNeuralNetwork();
	~CNeuralNetwork();

	virtual void Run();
};
#endif