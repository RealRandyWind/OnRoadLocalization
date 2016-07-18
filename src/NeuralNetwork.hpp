#ifndef NEURALNETWORK_HPP
#define NEURALNETWORK_HPP

#define NEURALNETWORK_EPS 0.001f

template<
	typename Input,
	typename Output,
	typename Validation,
	typename Training,
	typename Optimization,
	typename ObjectInitializer
>
class NeuralNetwork
{
private:
	void* a_ptrState;
	unsigned int a_nBytesState;
protected:
	unsigned int a_eErrorCode;
public:
	NeuralNetwork()
	{
		a_ptrState = nullptr;
		a_nBytesState = 0;
	}

	~NeuralNetwork() {}

	virtual void ReInitialize(
			void* lState,
			ObjectInitializer oInitializer
		) { /* Do Nothing */ }

	virtual Output* Use(
			Input* lInputs, 
			Output* lOutputs
		) = 0;

	virtual Output* Validate(
			Input* lSamples, 
			Output* lSampleTargets, 
			Output* lReturns,
			unsigned int nSamples, 
			Validation oValidationParms
		) = 0;

	virtual void Train(
			Input* lSamples, 
			Output* lSampleTargets, 
			unsigned int nSamples, 
			Training oTrainingParms
		) = 0;
	
	virtual void Optimize(
			Optimization oOptimizationParms
		) = 0;

	unsigned int GetErrorCode() { return a_eErrorCode; }

	unsigned int GetBytesState() { return a_nBytesState; }

	void* SetState(void* ptrState, unsigned int nBytesState, ObjectInitializer oInitializer)
	{
		void* lOldState = a_ptrState;
		a_ptrState = ptrState;
		a_nBytesState = nBytesState;
		ReInitialize(ptrState, oInitializer);
		return lOldState;
	}

	void* GetState(unsigned int nOffset)
	{
		return (void*)((unsigned char*)a_ptrState + nOffset);
	}

	template<typename T = void>
	T* GetState(unsigned int nOffset, bool bIsBytes = true)
	{
		if (!bIsBytes) { return (T*)a_ptrState + nOffset; }
		return (T*)GetState(nOffset);
	}
};
#endif
